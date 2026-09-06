/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your information in the following struct.
 ********************************************************/
team_t team = {
    /* Your student ID */
    "20211562",
    /* Your full name*/
    "DongHyeop Lee",
    /* Your email address */
    "nightdrive@sogang.ac.kr",
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define WSIZE 4                                     // word 및 header/footer 크기 
#define DSIZE 8                                     // double word 크기
#define CHUNKSIZE  (1<<12)                          // heap 확장을 위한 기본 청크 크기 (4KB)

#define MAX(x, y) ((x) > (y)? (x) : (y))

// header/footer에서 크기 및 할당 비트 읽기/쓰기 
#define PACK(size, alloc)  ((size) | (alloc))       // 크기와 할당 비트를 결합  
#define GET(p)       (*(unsigned int *)(p))         // 주소 p에서 워드 읽기 
#define PUT(p, val)  (*(unsigned int *)(p) = (val)) // 주소 p에 워드 쓰기 

// 주소 p에서 크기와 할당된 비트 읽기 
#define GET_SIZE(p)  (GET(p) & ~0x7)                // head/foot에서 크기 추출 
#define GET_ALLOC(p) (GET(p) & 0x1)                 // head/foot에서 할당 비트 추출 

// bp로부터 header 및 footer 주소 계산 
#define HDRP(bp)       ((char *)(bp) - WSIZE)                       // 페이로드 포인터에서 header 포인터 계산 
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)  // 페이로드 포인터에서 footer 포인터 계산 

// bp로부터 다음, 이전 블록 포인터 계산 
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) // 현재 블록의 다음 블록 페이로드 포인터 
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) // 현재 블록의 이전 블록 페이로드 포인터 

#define PRED_PTR(bp)   ((char *)(bp))                                   // free 블록 bp에서 이전 포인터 저장 위치 
#define SUCC_PTR(bp)   ((char *)(bp) + WSIZE)                           // free 블록 bp에서 다음 포인터 저장 위치 

#define GET_PRED(bp)   (*(char **)(bp))                                 // bp의 이전 가용 블록 주소 읽기 
#define GET_SUCC(bp)   (*(char **)(SUCC_PTR(bp)))                       // bp의 다음 가용 블록 주소 읽기 
#define SET_PRED(bp, val)  (*(char **)(bp) = (char *)(val))
#define SET_SUCC(bp, val)  (*(char **)(SUCC_PTR(bp)) = (char *)(val))

#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)                   // 요청 크기를 ALIGNMENT에 맞게 정렬
#define OVERHEAD DSIZE                                                  // header와 footer의 크기
#define MIN_ALLOC_SIZE (2 * DSIZE + DSIZE)                              // 최소 할당 크기 (head+foot(8) + payload(8) + 최소 free 블록 크기 (8))
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


static char *heap_listp; // heap의 시작을 가리키는 포인터 
static char *free_listp; // explicit free list의 head 포인터 

static void *extend_heap(size_t words);         // heap을 확장하여 free 블록 생성
static void *coalesce(void *bp);                // 인접한 free 블록과 병합하여 fragmentation 감소
static void *find_fit(size_t asize);            // Best-Fit 방식으로 free 블록 검색
static void place(void *bp, size_t asize);      // free 블록에 요청 크기만큼 할당하고 필요시 분할
static void add_to_free_list(void *bp);         // LIFO 방식으로 free 블록을 free list에 추가
static void remove_from_free_list(void *bp);    // free 블록을 free list에서 제거

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // empty heap 생성: padding + 프롤로그 header + 프롤로그 footer + 에필로그 header 
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                             // 패딩 
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    // 프롤로그 header (크기=8바이트, 할당됨) 
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    // 프롤로그 footer
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));        // 에필로그 header (크기=0, 할당됨) 
    free_listp = NULL;                              // explicit free list 초기화
     
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)       // CHUNKSIZE 크기로 heap 확장 및 초기 free 블록 생성
        return -1;

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;      // 조정된 블록 크기 
    size_t extendsize; // heap에 맞지 않을 경우 확장할 크기 
    char *bp;          // 할당할 블록의 포인터

    if (size == 0)  // 요청 크기가 0이면 
        return NULL;
    
    if (size <= DSIZE)          // 요청 크기가 8바이트 이하라면
        asize = MIN_ALLOC_SIZE; // 최소 24바이트 할당
    else                        // 요청 크기가 8바이트 초과라면
        asize = ALIGN(size + OVERHEAD); // 요청 크기 + header/footer 오버헤드 8바이트를 ALIGNMENT에 맞춤

    // 가용 리스트에서 적합한 블록 검색 (Best-Fit) 
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);                   /* 찾은 블록 할당 및 필요시 분할 */
        return bp;
    }

    // 적합한 블록을 찾지 못함. heap 확장 후 새로운 블록 할당 
    extendsize = MAX(asize, CHUNKSIZE);                 // 요청 크기와 CHUNKSIZE 중 큰 값으로 확장 크기 설정
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize); // 확장된 heap에서 블록 할당

    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr)); // 블록의 크기 가져오기
    PUT(HDRP(ptr), PACK(size, 0)); // header를 free로 표시 
    PUT(FTRP(ptr), PACK(size, 0)); // footer를 free로 표시 
    coalesce(ptr); // 인접한 free 블록과 병합
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *newptr;
    size_t oldsize;
    size_t new_asize;
    // 1. ptr이 NULL이면 mm_malloc(size)와 동일
    if (ptr == NULL) {
        return mm_malloc(size);
    }
    // 2. size가 0이면 mm_free(ptr)와 동일
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }
    oldsize = GET_SIZE(HDRP(ptr)); // 기존 블록의 전체 크기 

    if (size <= DSIZE)                      // 요청 크기가 8바이트 이하라면
        new_asize = 2 * DSIZE;              // 최소 16바이트 할당 (header + footer + 최소 free 블록 크기)
    else
        new_asize = ALIGN(size + OVERHEAD); // 요청 크기 + header/footer 오버헤드를 ALIGNMENT에 맞춤

    
    if (new_asize <= oldsize) {                         // 새 size가 기존 블록의 크기와 동일하거나 더 작다면 
        if ((oldsize - new_asize) >= (2 * DSIZE)) {     // 남은 공간이 최소 블록 크기보다 크다면
            // 현재 블록의 크기를 새 요청 크기로 줄임
            PUT(HDRP(ptr), PACK(new_asize, 1));
            PUT(FTRP(ptr), PACK(new_asize, 1));
            
            // 남은 공간을 새로운 free 블록으로 만듦
            void *new_free_bp = NEXT_BLKP(ptr);                     // 새로 생성될 free 블록의 payload 포인터
            PUT(HDRP(new_free_bp), PACK(oldsize - new_asize, 0));   // 새 free 블록의 header 설정
            PUT(FTRP(new_free_bp), PACK(oldsize - new_asize, 0));   // 새 free 블록의 footer 설정
            add_to_free_list(new_free_bp);                          // 새로 생성된 free 블록을 free list에 추가
        }
        return ptr;
    } else { // 새 size 가 기존 블록보다 더 크다면 
        size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr))); // 다음 블록의 할당 여부 
        size_t current_and_next_size = oldsize + GET_SIZE(HDRP(NEXT_BLKP(ptr))); // 현재 블록과 다음 블록을 합친 크기 

        // 다음 블록이 free 상태이고, 두 블록을 합치면 요청 크기를 만족한다면 
        if (!next_alloc && (current_and_next_size >= new_asize)) {
            remove_from_free_list(NEXT_BLKP(ptr));          // 다음 free 블록을 free list에서 제거 
            PUT(HDRP(ptr), PACK(current_and_next_size, 1)); // 현재 블록의 header 업데이트
            PUT(FTRP(ptr), PACK(current_and_next_size, 1)); // 새 footer 업데이트 
            return ptr;                                     // 기존 포인터 return
        } 
            newptr = mm_malloc(size);                       // 새로운 블록 할당
            if (newptr == NULL) {                           // 할당 실패 시 NULL 반환
                return NULL;
            }
            size_t old_payload_size = GET_SIZE(HDRP(ptr)) - DSIZE;                  // 기존 블록의 payload 크기 (header/footer 제외)
            size_t copy_size = (size < old_payload_size) ? size : old_payload_size; // 새 요청 크기와 기존 페이로드 크기 중 작은 값으로 복사 크기 결정
            memcpy(newptr, ptr, copy_size);                 // 기존 블록의 payload를 새 블록으로 복사
            mm_free(ptr);                                   // 기존 블록 free
            return newptr;
        }
    
}

// 워드 단위로 힙을 확장
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;
    //double word 정렬을 유지하며 heap 확장 
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;   
    if ((long)(bp = mem_sbrk(size)) == -1)                      // 힙 확장 실패 시 NULL 반환
        return NULL;
    // 새로운 free 블록 header/footer 설정 및 에필로그 헤더 이동 
    PUT(HDRP(bp), PACK(size, 0));         // free 블록 header 
    PUT(FTRP(bp), PACK(size, 0));         // free 블록 footer 
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // 새로운 에필로그 header

    // 이전 블록이 free 상태였다면 병합
    return coalesce(bp);
}


// 인접한 free 블록과 병합하여 fragmentation 줄이기
static void *coalesce(void *bp)
{
    assert(bp != NULL);                             // bp가 NULL이 아니어야 함
    assert((char *)bp >= (char *)mem_heap_lo());    // bp가 힙의 시작보다 크거나 같아야 함
    assert((char *)bp < (char *)mem_heap_hi());     // bp가 힙의 끝보다 작아야 함

    
    if ((char *)bp < heap_listp + 2 * WSIZE) {      // 최소 블록 크기보다 작은 주소면 NULL
        return NULL;
    }

    size_t prev_alloc;                                  // 이전 블록 할당 여부
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))); // 다음 블록 할당 여부
    size_t size = GET_SIZE(HDRP(bp));                   // 현재 블록 크기   

    char *first_payload = heap_listp + 2 * WSIZE;       // 프롤로그 footer 다음이 첫 payload
    if ((char *)bp <= first_payload) {
        prev_alloc = 1;                                 // 프롤로그 블록이거나 그 이전이면 할당된 것
    } else {                                            // 이전 블록이 할당된 상태인지 확인
        char *prev_bp = PREV_BLKP(bp);                  // 현재 블록의 이전 블록 포인터
        prev_alloc = GET_ALLOC(FTRP(prev_bp));          // 이전 블록의 footer에서 할당 여부 읽기
    }

    if (prev_alloc && next_alloc) {             // 1: 이전/다음 블록 모두 할당됨 
        add_to_free_list(bp);                   // 현재 블록만 free 리스트에 추가 
        return bp;
    }

    else if (prev_alloc && !next_alloc) {       //2: 이전 블록 할당, 다음 블록 free
        remove_from_free_list(NEXT_BLKP(bp));   // 다음 free 블록을 리스트에서 제거
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));  // 현재 블록 크기에 다음 블록 크기 추가 
        PUT(HDRP(bp), PACK(size, 0));           // 현재 블록 header 업데이트 
        PUT(FTRP(bp), PACK(size, 0));           // 다음 블록 footer에 새로운 footer 쓰기 
        add_to_free_list(bp);                   // 병합된 현재 블록을 free 리스트에 추가 
    }

    else if (!prev_alloc && next_alloc) {           // 3: 이전 블록 free, 다음 블록 할당 
        remove_from_free_list(PREV_BLKP(bp));       // 이전 free 블록을 리스트에서 제거 
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));      // 현재 블록 크기에 이전 블록 크기 추가 
        PUT(FTRP(bp), PACK(size, 0));               // 현재 블록 footer 업데이트 
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));    // 이전 블록 header에 새로운 header 쓰기 
        bp = PREV_BLKP(bp);                         // 블록 포인터를 이전 블록으로 이동 
        add_to_free_list(bp);                       // 병합된 이전 블록을 free 리스트에 추가 
    }

    else {                                          // 4: 이전/다음 블록 모두 가용 
        remove_from_free_list(PREV_BLKP(bp));       // 이전 free 블록을 리스트에서 제거 
        remove_from_free_list(NEXT_BLKP(bp));       // 다음 free 블록을 리스트에서 제거 
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp))); // 세 블록의 크기 더하기
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));    // 이전 블록 header에 새로운 header 쓰기 
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));    // 다음 블록 footer에 새로운 footer 쓰기 
        bp = PREV_BLKP(bp);                         // 블록 포인터를 이전 블록으로 이동 
        add_to_free_list(bp);                       // 병합된 블록을 free 리스트에 추가 
    }
    return bp;
}


// Best-Fit 방식으로 free 리스트에서 asize를 가질 수 있는 블록을 찾기
static void *find_fit(size_t asize)
{
    void *bp;
    void *best_fit_bp = NULL;       // 가장 적합한 블록을 저장할 포인터
    size_t min_diff = (size_t)-1;   // 최소 차이를 저장 (최대값으로 초기화)

    // explicit free list를 traverse하며 적합한 블록 검색 
    for (bp = free_listp; bp != NULL; bp = GET_SUCC(bp)) {      
        size_t current_size = GET_SIZE(HDRP(bp));
        if (current_size >= asize) {            // 현재 블록이 요청 크기를 수용할 수 있는 경우
            size_t diff = current_size - asize; // 요청 크기와의 차이 계산
            if (diff < min_diff) {              // 현재 블록이 이전에 찾은 best_fit_bp보다 더 적합한 경우
                min_diff = diff;
                best_fit_bp = bp;
            }
        }
    }
    return best_fit_bp; // 가장 적합한 블록의 포인터 반환
}

// 찾은 free 블록 bp에 asize 크기의 할당된 블록을 배치하고 남은 공간이 있다면 새로운 free 블록으로 분할 
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));          // 현재 free 블록의 크기 
    remove_from_free_list(bp);                  // free 리스트에서 현재 블록 제거 
    if ((csize - asize) >= (MIN_ALLOC_SIZE)) {  // 블록 분할이 가능한 경우
        PUT(HDRP(bp), PACK(asize, 1));          // 할당된 블록 header 설정 
        PUT(FTRP(bp), PACK(asize, 1));          // 할당된 블록 footer 설정 
        memset(bp, 0, DSIZE);                   // 블록 초기화 
        bp = NEXT_BLKP(bp);                     // 다음 블록으로 이동 
        PUT(HDRP(bp), PACK(csize - asize, 0));  // 남은 공간에 대한 free 블록 header 설정 
        PUT(FTRP(bp), PACK(csize - asize, 0));  // 남은 공간에 대한 free 블록 footer 설정 
        coalesce(bp);                           // 분할된 새 free 블록 병합
    } else {                                    // 분할할 수 없는 경우 (내부 fragmentation 발생) 
        PUT(HDRP(bp), PACK(csize, 1));          // 전체 블록을 할당된 것으로 표시 
        PUT(FTRP(bp), PACK(csize, 1));          // footer도 할당된 것으로 표시
        memset(bp, 0, DSIZE);                   // 블록 초기화
    }
}

// LIFO 방식으로 free 블록을 free 리스트 head에 추가
static void add_to_free_list(void *bp)
{
    SET_SUCC(bp, free_listp); // 새 블록의 다음 포인터는 현재 헤드
    if (free_listp != NULL) { // 리스트가 비어있지 않았다면, 이전 헤드의 이전 포인터 업데이트
        SET_PRED(free_listp, bp);
    }
    SET_PRED(bp, NULL);       // 새 블록은 새로운 헤드이므로 이전 포인터는 NULL
    free_listp = bp;          // 헤드 포인터 업데이트
}


// free 블록을 free 리스트에서 제거합니다.
static void remove_from_free_list(void *bp)
{
    if (GET_PRED(bp) == NULL) {                     // 리스트의 헤드인 경우 
        free_listp = GET_SUCC(bp);      
        if (GET_SUCC(bp) != NULL) {                 // 다음 블록이 있다면 
            SET_PRED(GET_SUCC(bp), NULL);
        }
    } else {                                        // 리스트의 중간 또는 끝에 있는 경우 
        SET_SUCC(GET_PRED(bp), GET_SUCC(bp));       // 이전 블록의 다음 포인터 업데이트 
        if (GET_SUCC(bp) != NULL) {                 // 다음 블록이 있다면 
            SET_PRED(GET_SUCC(bp), GET_PRED(bp));   // 다음 블록의 이전 포인터 업데이트 
        }
    }
}
