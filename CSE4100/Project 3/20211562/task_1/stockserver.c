#include "csapp.h"

#define MAX_CLIENT 1000

struct tree_node {              // bst node
    int id;                     // id   
    int left_stock;             // left stock on market
    int price;                  // stock price  
    int readcnt;                // read count
    sem_t mutex;                // mutex for critical section

    struct tree_node *left;     // left child node
    struct tree_node *right;    // right child node
};

struct tree_node *root = NULL;          // bst root node
static int active_clients = 0;          // Current number of active clients
static sem_t client_cnt_mutex;       

void sigint_handler(int sig);
void load_stock(const char* filename);
void insert_stock(int id, int left, int price);
void save_stock_recursive(FILE *fp, struct tree_node *node);
void save_stock(const char* filename);
struct tree_node* find_stock(int id);
void inorder(struct tree_node *node, char *buffer, size_t bufsize);
void handle_command(int connfd, char* buf);
void padded_write(int connfd, const char *msg);

void sigint_handler(int sig) {                          // SIGINT handler
    save_stock("stock.txt");                            // Save stocks to file 
    printf("Server shutting down. Stocks saved.\n");  
    exit(0);
}

void load_stock(const char* filename){                  // load stocks.txt
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        exit(1);
    }

    int id, stock, price;
    while (fscanf(fp, "%d %d %d", &id, &stock, &price) != EOF) {    //read stock data from file
        insert_stock(id, stock, price);                             // insert stock into binary search tree       
    }

    fclose(fp);    
}   

void insert_stock(int id, int left, int price) {                // insert stock into binary search tree
    struct tree_node *new = malloc(sizeof(struct tree_node));   // init new node
    new->id = id;                                                               
    new->left_stock = left;
    new->price = price;
    new->readcnt = 0;
    Sem_init(&new->mutex, 0, 1);                                // init mutex for node
    new->left = new->right = NULL;

    if (root == NULL) {                                         // empty tree
        root = new;
        return;
    }

    struct tree_node *cur = root, *parent = NULL;               
    while (cur != NULL) {                                       // traverse 
        parent = cur;                                           // update parent node    
        if (id < cur->id) {                                          
            cur = cur->left;                                    // move to left child    
        } else if (id > cur->id) {                                 
            cur = cur->right;                                   // move to right child    
        } else {
            free(new);                                          // if id already exists, free new node 
            return;
        }
    }

    if (id < parent->id)                                        // if id is less than parent's id                   
        parent->left = new;                                     // set new node as left child    
    else                                                        
        parent->right = new;                                    // set new node as right child  
}

void save_stock_recursive(FILE *fp, struct tree_node *node) {           // save stocks to file recursively
    if (!node) return;                                                  // if node is NULL, return
    save_stock_recursive(fp, node->left);                               // recursively save left subtree   
    fprintf(fp, "%d %d %d\n", node->id, node->left_stock, node->price); // write node data to file
    save_stock_recursive(fp, node->right);                              // recursively save right subtree
}

void save_stock(const char* filename){                                  // save stock at file
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        return;
    }
    save_stock_recursive(fp, root);                                      // save stocks recursively to file  
    fclose(fp);
}        

struct tree_node* find_stock(int id) {                                  // Find stock by id in bst
    struct tree_node *cur = root;                                       // start from root 
    while (cur) {                                                       // traverse    
        if (id == cur->id) return cur;                                  // if id matches, return node 
        else if (id < cur->id) cur = cur->left;                         // if id is less, move to left child    
        else cur = cur->right;                                          // if id is greater, move to right child
    }
    return NULL;
}

void inorder(struct tree_node *node, char *buffer, size_t bufsize) {                        // Inorder traversal 
    char line[64];
    if (!node) return;
    inorder(node->left, buffer, bufsize);                                                   // recursive traverse left subtree
    snprintf(line, sizeof(line), "%d %d %d\n", node->id, node->left_stock, node->price);    // format node data into line
    strncat(buffer, line, bufsize - strlen(buffer) - 1);                                    // append line to buffer, ensuring null termination
    inorder(node->right, buffer, bufsize);                                                  // recursive traverse right subtree 
}

void padded_write(int connfd, const char *msg) {            // Write message to client with padding 
    char buf[MAXLINE] = {0};                                // init buffer  
    strncpy(buf, msg, MAXLINE - 1);                         // copy message to buffer, ensuring null termination    
    Rio_writen(connfd, buf, MAXLINE);                       // write buffer to connection
}

void handle_command(int connfd, char* buf){                 // Handle client command 
    char cmd[10];
    int id, amount;
    sscanf(buf, "%s", cmd);                                 // parse command from buffer

    if (!strcmp(cmd, "show")){                          
        char bigbuf[MAXLINE] = {0};
        inorder(root, bigbuf, sizeof(bigbuf));              // inorder traverse the tree and accumulate data into bigbuf
        padded_write(connfd, bigbuf);                       // write accumulated data to client
    } 
    else if (!strcmp(cmd, "buy")){               
        sscanf(buf, "%*s %d %d", &id, &amount);             // parse id and amount from buffer
        struct tree_node *node = find_stock(id);            // find stock by id

        if (!node) {
            padded_write(connfd, "Invalid ID\n");  
            return;
        }

        P(&node->mutex);                                        // Enter critical section for stock node 
        if (node->left_stock < amount) {                        // if not enough left stocks
            padded_write(connfd, "Not enough left stock\n");  
        } else {                                                // if enough left stocks
            node->left_stock -= amount;                         // minus left stock    
            padded_write(connfd, "[buy] success\n"); 
        }
        V(&node->mutex);                                        // Exit critical section 
    } 
    else if (!strcmp(cmd, "sell")){           
        sscanf(buf, "%*s %d %d", &id, &amount);                 // parse id and amount from buffer
        struct tree_node *node = find_stock(id);                // find stock by id

        if (!node) {
            padded_write(connfd, "Invalid ID\n"); 
            return;
        }

        P(&node->mutex);                            // Enter critical section for stock node        
        node->left_stock += amount;                 // add left stock   
        padded_write(connfd, "[sell] success\n");            
        V(&node->mutex);                            // Exit critical section for stock node 
    }
}


int main(int argc, char **argv) {
    //printf("Server started\n");
    int listenfd, connfd;
    int clientfd[MAX_CLIENT];  
    rio_t clientrio[MAX_CLIENT];  

    fd_set read_set, ready_set;                         // Set of file descriptors for select
    int maxfd, maxi;                                    // Maximum file descriptor and index of the largest fd in clientfd array
    int i, n;                      
    char buf[MAXLINE];                                  // Buffer for reading lines from clients
    socklen_t clientlen;                                // Length of client address structure
    struct sockaddr_storage clientaddr;                 // Client address structure
    char client_hostname[MAXLINE], client_port[MAXLINE];    // Buffers for client hostname and port

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    signal(SIGINT, sigint_handler);                     // signal handler installation
    load_stock("stock.txt");                            // Load stocks from file        
    Sem_init(&client_cnt_mutex, 0, 1);                  // Initialize mutex for client count  

    listenfd = Open_listenfd(argv[1]);                  // Open a listening socket

    maxfd = listenfd;                                   // Initialize maxfd to the listening socket
    maxi = -1;                                             
    for (i = 0; i < MAX_CLIENT; i++)                    // Initialize client fd
        clientfd[i] = -1;         

    FD_ZERO(&read_set);                                 // Clear the read set    
    FD_SET(listenfd, &read_set);                        // Add the listening socket to the read set 

    while (1) {
        ready_set = read_set;                           // Copy read_set to ready_set for select

        if (Select(maxfd + 1, &ready_set, NULL, NULL, NULL) < 0) {  // Wait for command on the file descriptors
            if (errno == EINTR) continue;                           // If interrupted by a signal, continue
            else {
                perror("select error");
                exit(1);
            }
        }

        if (FD_ISSET(listenfd, &ready_set)){                        // If the listening socket is ready, accept a new connection
            clientlen = sizeof(struct sockaddr_storage);            // Initialize client address length
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);   // Accept a new connection
            Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);  // Get client hostname and port
            //printf("Connected to (%s, %s)\n", client_hostname, client_port);    
            for (i = 0; i < MAX_CLIENT; i++) {              // Find an empty slot in clientfd array
                if (clientfd[i] < 0) {                      // If an empty slot is found
                    clientfd[i] = connfd;                   // Store the new connection fd
                    Rio_readinitb(&clientrio[i], connfd);   // Initialize rio structure for the new connection
                    
                    P(&client_cnt_mutex);
                    active_clients++;                   // Increment active client count
                    V(&client_cnt_mutex);
                    break;
                }
            }
            FD_SET(connfd, &read_set);                      // Add the new connection fd to the read set    
            if (connfd > maxfd) maxfd = connfd;             // Update maxfd if the new connection fd is larger
            if (i > maxi) maxi = i;                         // Update maxi if the index of the new connection fd is larger        
        }
        for (i = 0; i <= maxi; i++) {                       // Check each client fd in the clientfd array   
            int sockfd;
            if ((sockfd = clientfd[i]) < 0)                 // If the client fd is invalid, continue to the next iteration
                continue;

            if (FD_ISSET(sockfd, &ready_set)) {             // If the client fd is ready for reading
                if ((n = Rio_readlineb(&clientrio[i], buf, MAXLINE)) <= 0) {    // Read a line from the client
                    Close(sockfd);                                              // Close the client connection
                    FD_CLR(sockfd, &read_set);                                  // Remove the client fd from the read set
                    clientfd[i] = -1;                                           // Mark the client fd as invalid
                    
                    P(&client_cnt_mutex);
                    active_clients--;        // client num decrement
                    if (active_clients == 0) {
                        save_stock("stock.txt");  // save stock data if no active clients
                    }
                    V(&client_cnt_mutex);
                } 
                else {
                    printf("server received %d bytes\n", n);
                    handle_command(sockfd, buf);                                // Handle the command received from the client
                }
            }
        }
    }
    return 0;
}

