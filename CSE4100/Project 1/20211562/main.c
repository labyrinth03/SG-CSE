#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"

// 각 변수 크기 조절을 위한 definition
#define MAX_NAME 32
#define MAX_CMD 100
#define MAX_STRUCTS 10

// 자료구조 개수 카운트
int list_num = 0;
int hash_num = 0;
int bitmap_num = 0;

struct bitmap_array{            //각 자료구조 저장을 위한 구조체
    char name[MAX_NAME];
    struct bitmap *bitmap;
};
struct hash_array{
    char name[MAX_NAME];
    struct hash *hash;
};
struct list_array{
    char name[MAX_NAME];
    struct list *list;
};

bool tf_to_bool(char *tf){                  //true false bool 타입으로
    if(strcmp(tf, "true") == 0){
        return true;
    }
    else if(strcmp(tf, "false") == 0){
        return false;
    }
    else{
        return false;
    }
}

char* bool_to_tf(bool value){               //bool 값 true false로
    if(value == true){
        return "true";
    }
    else{
        return "false";
    }
}

void handle_command(FILE *fp) {
    int bitmap_flag = 0;                                //현재 파일이 어떤 자료구조 다루는지 판단을 위한 flag
    int hash_flag = 0;
    int list_flag = 0;

    struct bitmap_array bitmap_entries[MAX_STRUCTS];    //각 자료구조 저장을 위한 구조체 배열
    struct hash_array hash_entries[MAX_STRUCTS];
    struct list_array list_entries[MAX_STRUCTS];

    int index1, index2, index3;                         //각 명령들이 넘기는 인자 저장을 위한 변수들
    char line[100];
    char name[20], name2[20];
    size_t bit_index;
    size_t bit_count;
    size_t bit_size;
    size_t start_index, end_index;
    char tf[10];
    char hash_action[10]; int hash_value;
    int list_index; int list_value;

    while(fgets(line, sizeof(line), fp)){
        if(strcmp(line, "quit\n") == 0){                //quit 입력되면 종료
            return;
        }
            else if (sscanf(line, "create bitmap %s %zu", name, &bit_count) == 2) {
                bitmap_flag = 1;                                                                //비트맵 flag set
                strcpy(bitmap_entries[bitmap_num].name, name);                                  //배열에 비트맵 이름 저장
                bitmap_entries[bitmap_num++].bitmap = bitmap_create(bit_count);                 //배열에 새로운 비트맵 생성
            }
            else if (sscanf(line, "dumpdata %s", name) == 1) {
                if(bitmap_flag){                                                                //비트맵 출력
                    for(int i = 0; i < bitmap_num; i++){
                        if(strcmp(bitmap_entries[i].name, name) == 0){                          //이름이 동일하면
                            bitmap_print(bitmap_entries[i].bitmap);                             //함수 실행
                        }
                    }
                }
                else if(hash_flag){                                                             //해시 출력
                    for(int i = 0; i < hash_num; i++){
                        if(strcmp(hash_entries[i].name, name) == 0){
                            hash_print(hash_entries[i].hash);
                        }
                    }
                }
                else if(list_flag){                                                             //리스트 출력
                    for(int i = 0; i < list_num; i++){
                        if(strcmp(list_entries[i].name, name) == 0){
                            list_print(list_entries[i].list);
                        }
                    }
                }
            }
            else if (sscanf(line, "delete %s", name) == 1){
                if(bitmap_flag){                                                                 //비트맵 삭제
                    for(int i = 0; i < bitmap_num; i++){
                        if(strcmp(bitmap_entries[i].name, name) == 0){
                            bitmap_destroy(bitmap_entries[i].bitmap);
                        }
                    }
                    bitmap_num--;                                                                //개수 줄이기
                }
                else if(hash_flag){                                                              //해시 삭제
                    for(int i = 0; i < hash_num; i++){
                        if(strcmp(hash_entries[i].name, name) == 0){
                            hash_destroy(hash_entries[i].hash, data_destructor);
                        }
                    }
                    hash_num--;                                                                  //개수 줄이기
                }
                else if(list_flag){                                                              //리스트 삭제
                    for(int i = 0; i < list_num; i++){
                        if(strcmp(list_entries[i].name, name) == 0){
                            list_destroy(list_entries[i].list);
                        }
                    }
                    list_num--;                                                                  //개수 줄이기
                }
            }
            else if (sscanf(line, "bitmap_mark %s %zu", name, &bit_index) == 2) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_mark(bitmap_entries[i].bitmap, bit_index);                        //bitmap_mark 실행
                    }
                }
            }
            else if (sscanf(line, "bitmap_all %s %zu %zu", name, &start_index, &end_index) == 3) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%s\n", bool_to_tf(bitmap_all(bitmap_entries[i].bitmap, start_index, end_index)));       //bitmap_all 결과 출력
                    }
                }
            }
            else if (sscanf(line, "bitmap_any %s %zu %zu", name, &start_index, &end_index) == 3) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%s\n", bool_to_tf(bitmap_any(bitmap_entries[i].bitmap, start_index, end_index)));       //bitmap_any 결과 출력
                    }
                }
            }
            else if (sscanf(line, "bitmap_contains %s %zu %zu %s", name, &bit_index, &bit_count, tf) == 4) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%s\n", bool_to_tf(bitmap_contains(bitmap_entries[i].bitmap, bit_index, bit_count, tf_to_bool(tf))));        //bitmap_contains 결과 출력
                    }
                }
            }
            else if(sscanf(line, "bitmap_count %s %zu %zu %s", name, &start_index, &end_index, tf) == 4){
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%zu\n",bitmap_count(bitmap_entries[i].bitmap, start_index, end_index, tf_to_bool(tf)));                     //bitmap_count 결과 출력
                    }
                }
            }
            else if(sscanf(line, "bitmap_dump %s", name) == 1){
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_dump(bitmap_entries[i].bitmap);                                                                              //bitmap_dump 실행
                    }
                }
            }
            else if(sscanf(line, "bitmap_expand %s %zu", name, &bit_size) == 2){    
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_entries[i].bitmap = bitmap_expand(bitmap_entries[i].bitmap, bit_size);                                       //bitmap_expand 실행
                    }
                }
            }
            else if(sscanf(line, "bitmap_set_all %s %s", name, tf) == 2){
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_set_all(bitmap_entries[i].bitmap, tf_to_bool(tf));                                                           //bitmap_set_all 실행
                    }
                }
            }
            else if(sscanf(line, "bitmap_flip %s %zu", name, &bit_index) == 2){
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_flip(bitmap_entries[i].bitmap, bit_index);                                                                   //bitmap_flip 실행
                    }
                }                
            }
            else if(sscanf(line, "bitmap_none %s %zu %zu", name, &start_index, &end_index) == 3){
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%s\n", bool_to_tf(bitmap_none(bitmap_entries[i].bitmap, start_index, end_index)));                          //bitmap_none 결과 출력
                    }
                }
            }
            else if (sscanf(line, "bitmap_reset %s %zu", name, &bit_index) == 2) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_reset(bitmap_entries[i].bitmap, bit_index);                                                                  //bitmap_reset 실행
                    }
                }
            }
            else if (sscanf(line, "bitmap_scan_and_flip %s %zu %zu %s", name, &start_index, &end_index, tf) == 4) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%zu\n",bitmap_scan_and_flip(bitmap_entries[i].bitmap, start_index, end_index, tf_to_bool(tf)));             //bitmap_scan_and_flip 실행
                    }
                }
            }
            else if (sscanf(line, "bitmap_scan %s %zu %zu %s", name, &start_index, &end_index, tf) == 4) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%zu\n",bitmap_scan(bitmap_entries[i].bitmap, start_index, end_index, tf_to_bool(tf)));                      //bitmap_scan 실행
                    }
                }
            }
            else if (sscanf(line, "bitmap_set_all %s %s", name, tf) == 2) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_set_all(bitmap_entries[i].bitmap, tf_to_bool(tf));                                                           //bitmap_set_all 실행
                    }
                }
            }
            else if (sscanf(line, "bitmap_set_multiple %s %zu %zu %s", name, &start_index, &end_index, tf) == 4) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_set_multiple(bitmap_entries[i].bitmap, start_index, end_index, tf_to_bool(tf));                              //bitmap_set_multiple 실행
                    }
                }
            }
            else if (sscanf(line, "bitmap_set %s %zu %s", name, &bit_index, tf) == 3) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        bitmap_set(bitmap_entries[i].bitmap, bit_index, tf_to_bool(tf));                                                    //bitmap_set 실행                                       
                    }
                }
            }
            else if (sscanf(line, "bitmap_size %s", name) == 1) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%zu\n",bitmap_size(bitmap_entries[i].bitmap));                                                              //bitmap_size 결과 출력
                    }
                }
            }
            else if (sscanf(line, "bitmap_test %s %zu", name, &bit_index) == 2) {
                for(int i = 0; i < bitmap_num; i++){
                    if(strcmp(bitmap_entries[i].name, name) == 0){
                        printf("%s\n", bool_to_tf(bitmap_test(bitmap_entries[i].bitmap, bit_index)));                                       //bitmap_test 결과 출력
                    }
                }
            }           
            else if (sscanf(line, "create hashtable %s", name) == 1) {
                hash_flag = 1;                                                                                                              //hash flag set
                strcpy(hash_entries[hash_num].name, name);                                                                                  //새로운 hash 배열에 이름 저장
                hash_entries[hash_num++].hash = hash_create(hash_hash, hash_less, NULL);                                                    //새로운 hash 배열에 저장
            }
            else if (sscanf(line, "hash_insert %s %d", name, &hash_value) == 2) {
                struct data *data = malloc(sizeof(struct data));
                data->value = hash_value;
                struct hash_elem *hash_elem = &data->elem;                                                                                  //insert할 data랑 hash_elem 준비
                for(int i = 0; i < hash_num; i++){
                    if(strcmp(hash_entries[i].name, name) == 0){
                        hash_insert(hash_entries[i].hash, hash_elem);                                                                       //hash_insert 실행
                    }
                }
            }
            else if (sscanf(line, "hash_apply %s %s", name, hash_action) == 2) {                                                            //hash_apply 명령 부분
                for(int i = 0; i < hash_num; i++){
                    if(strcmp(hash_entries[i].name, name) == 0){
                        if(strcmp(hash_action, "square") == 0){                                                                             //square 들어오면
                            hash_apply(hash_entries[i].hash, hash_square);                                                                  //hash_square 실행
                        }
                        else if(strcmp(hash_action, "triple") == 0){                                                                        //triple 들어오면
                            hash_apply(hash_entries[i].hash, hash_triple);                                                                  //hash_triple 실행
                        }
                    }
                }
            }
            else if (sscanf(line, "hash_delete %s %d", name, &hash_value) == 2) {
                struct data *data = malloc(sizeof(struct data));
                data->value = hash_value;
                struct hash_elem *hash_elem = &data->elem;                                                                                  //delete할 data랑 hash_elem 준비
                for(int i = 0; i < hash_num; i++){
                    if(strcmp(hash_entries[i].name, name) == 0){
                        hash_delete(hash_entries[i].hash, hash_elem);                                                                       //hash_delete 실행
                    }
                }
            }
            else if (sscanf(line, "hash_empty %s", name) == 1) {
                for(int i = 0; i < hash_num; i++){
                    if(strcmp(hash_entries[i].name, name) == 0){
                        printf("%s\n",bool_to_tf(hash_empty(hash_entries[i].hash)));                                                        //hash_empty 결과 출력
                    }
                }
            }
            else if (sscanf(line, "hash_size %s", name) == 1) {
                for(int i = 0; i < hash_num; i++){
                    if(strcmp(hash_entries[i].name, name) == 0){
                        printf("%zu\n",hash_size(hash_entries[i].hash));                                                                    //hash_size 결과 출력
                    }
                }
            }
            else if (sscanf(line, "hash_clear %s", name) == 1) {
                for(int i = 0; i < hash_num; i++){
                    if(strcmp(hash_entries[i].name, name) == 0){
                        hash_clear(hash_entries[i].hash, data_destructor);                                                                  //hash_clear 실행
                    }
                }
            }
            else if (sscanf(line, "hash_find %s %d", name, &hash_value) == 2) {
                struct data *data = malloc(sizeof(struct data));                                                                            //hash_find에 인자로 넣어줄 value가 담긴 hash_elem생성
                data->value = hash_value;
                struct hash_elem *hash_elem = &data->elem;
                for(int i = 0; i < hash_num; i++){
                    if(strcmp(hash_entries[i].name, name) == 0){
                        if(hash_find(hash_entries[i].hash, hash_elem) == NULL){                                                             //hash_find 했는데 없는 값이면
                            printf("\n");
                        }
                        else{                               
                            printf("%d\n", hash_value);                                                                                     //있는 값이면 hash_value 출력
                        }
                    }
                }
            }
            else if (sscanf(line, "hash_replace %s %d", name, &hash_value) == 2) {                                                          
                struct data *data = malloc(sizeof(struct data));                                                                            //replace 해줄 value가 담긴 hash_elem 생성
                data->value = hash_value;
                struct hash_elem *hash_elem = &data->elem;
                for(int i = 0; i < hash_num; i++){
                    if(strcmp(hash_entries[i].name, name) == 0){
                        hash_replace(hash_entries[i].hash, hash_elem);                                                                      //hash_replace 실행
                    }
                }
            }
            else if (sscanf(line, "create list %s", name) == 1) {
                list_flag = 1;                                                                                                              //list_flag set
                strcpy(list_entries[list_num].name, name);                                                                                  //배열에 list name 저장
                list_entries[list_num++].list = list_create();                                                                              //배열에 새로운 list 생성
            }

            else if (sscanf(line, "list_push_back %s %d", name, &list_value) == 2) {
                struct list_item *list_item = (struct list_item *)malloc(sizeof(struct list_item));                                         //push할 value 넣을 list_item 생성
                list_item->data = list_value;                                                                                               //data에 value 저장
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_push_back(list_entries[i].list, &list_item->elem);                                                              //list_push_back 실행
                    }
                }
            }  
            else if (sscanf(line, "list_front %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        struct list_elem *front_elem = list_front(list_entries[i].list);                                                     //list_front() 저장할 back_elem에서 data 가져오기
                        struct list_item *front_item = list_entry(front_elem, struct list_item, elem);  
                        printf("%d\n", front_item->data);                                                                                     //list_front 결과 출력
                    }
                }
            }     
            else if (sscanf(line, "list_back %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        struct list_elem *back_elem = list_back(list_entries[i].list);                                                          //list_back() 저장할 back_elem에서 data 가져오기
                        struct list_item *back_item = list_entry(back_elem, struct list_item, elem);
                        printf("%d\n", back_item->data);                                                                                        //list_back 결과 출력
                    }
                }
            }          
            else if (sscanf(line, "list_pop_back %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_pop_back(list_entries[i].list);                                                                                //list_pop_back 실행
                    }
                }
            }   
            else if (sscanf(line, "list_pop_front %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_pop_front(list_entries[i].list);                                                                               //list_pop_front 실행
                    }
                }
            }   
            else if (sscanf(line, "list_insert_ordered %s %d", name, &list_value) == 2) {
                struct list_item *list_item = malloc(sizeof(struct list_item));                                                             //insert할 value 저장할 list_item 만들기
                list_item->data = list_value;
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_insert_ordered(list_entries[i].list, &list_item->elem, list_less, NULL);                                       //list_insert_ordered 실행
                    }
                }
            } 
            else if (sscanf(line, "list_insert %s %d %d", name, &list_index, &list_value) == 3) {
                struct list_item *list_item = (struct list_item *)malloc(sizeof(struct list_item));                                         //insert할 value 저장한 list_item 만들기
                list_item->data = list_value;
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_insert(list_find(list_entries[i].list, list_index), &list_item->elem);                                         //list_insert 실행
                    }
                }
            }
            else if (sscanf(line, "list_empty %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        printf("%s\n",bool_to_tf(list_empty(list_entries[i].list)));                                                        //list_empty 결과 출력
                    }
                }
            } 
            else if (sscanf(line, "list_size %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        printf("%zu\n",list_size(list_entries[i].list));                                                                    //list_size 결과 출력
                    }
                }
            } 
            else if (sscanf(line, "list_max %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        struct list_elem *max_elem = list_max(list_entries[i].list, list_less, NULL);                                       //list_max로 max값이 담긴 list_elem 찾기
                        struct list_item *max_item = list_entry(max_elem, struct list_item, elem);
                        printf("%d\n", max_item->data);                                                                                     //list_max 결과 출력
                    }
                }
            } 
            else if (sscanf(line, "list_min %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        struct list_elem *min_elem = list_min(list_entries[i].list, list_less, NULL);                                       //list_min으로 min 값이 담긴 list_elem 찾기
                        struct list_item *min_item = list_entry(min_elem, struct list_item, elem);
                        printf("%d\n", min_item->data);                                                                                     //list_min 결과 출력
                    }
                }
            } 
            else if (sscanf(line, "list_push_front %s %d", name, &list_value) == 2) {
                struct list_item *list_item = malloc(sizeof(struct list_item));
                list_item->data = list_value;                                                                                               //push할 list_elem에 value 저장
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_push_front(list_entries[i].list, &list_item->elem);                                                            //list_push_front 실행
                    }
                }
            } 
            else if (sscanf(line, "list_remove %s %d", name, &list_index) == 2) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_remove(list_find(list_entries[i].list, list_index));                                                           //list_remove 실행
                    }
                }
            } 
            else if (sscanf(line, "list_reverse %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_reverse(list_entries[i].list);                                                                                 //list_reverse 실행
                    }
                }
            } 
            else if (sscanf(line, "list_shuffle %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_shuffle(list_entries[i].list);                                                                                 //list_shuffle 실행
                    }
                }
            } 
            else if (sscanf(line, "list_sort %s", name) == 1) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_sort(list_entries[i].list, list_less, NULL);                                                                   //list_sort 실행
                    }
                }
            } 
            else if (sscanf(line, "list_splice %s %d %s %d %d", name, &index1, name2, &index2, &index3) == 5) {
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        struct list_elem *list_elem_1;
                        struct list_elem *list_elem_2;
                        struct list_elem *list_elem_3;
                        for(int j = 0; j < list_num; j++){
                            if(strcmp(list_entries[j].name, name) == 0){
                                list_elem_1 = list_find(list_entries[j].list, index1);                                                      //list_elem before에 넘길 list_elem
                            }
                            if(strcmp(list_entries[j].name, name2) == 0){
                                list_elem_2 = list_find(list_entries[j].list, index2);                                                      //list_elem first에 넘길 list_elem
                                list_elem_3 = list_find(list_entries[j].list, index3);                                                      //list_elem last에 넘길 list_elem
                            }
                        }
                        list_splice(list_elem_1, list_elem_2, list_elem_3);                                                                 //list_splice 실행
                    }
                }
            } 
            else if (sscanf(line, "list_swap %s %zu %zu", name, &start_index, &end_index) == 3) {                                           
                for(int i = 0; i < list_num; i++){
                    struct list_elem *list_elem_1;
                    struct list_elem *list_elem_2;
                    if(strcmp(list_entries[i].name, name) == 0){
                        list_elem_1 = list_find(list_entries[i].list, start_index);                                                          //swap할 첫번째 elem 찾기
                        list_elem_2 = list_find(list_entries[i].list, end_index);                                                            //swap할 두번째 elem 찾기
                        list_swap(list_elem_1, list_elem_2);                                                                                 //list_swap 실행
                    }
                }
            } 
            else if (sscanf(line, "list_unique %s %s", name, name2) == 2) {                                                                 //list_unique 인자 두 개인 경우
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        for(int j = 0; j < list_num; j++){
                            if(strcmp(list_entries[j].name, name2) == 0){
                                list_unique(list_entries[i].list, list_entries[j].list, list_less, NULL);                                   //list_unique 두 list 찾아서 실행
                            }
                        }
                    }
                }
            }
            else if (sscanf(line, "list_unique %s", name) == 1) {                                                                           //list_unique 인자 하나인 경우
                for(int i = 0; i < list_num; i++){
                    if(strcmp(list_entries[i].name, name) == 0){
                        struct list *list = list_create();
                        list_unique(list_entries[i].list, list, list_less, NULL);
                    }
                }
            }
    }
}

int main() {
    handle_command(stdin);  //stdin으로 handle_command 실행
    return 0;
}