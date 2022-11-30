
#include "event.h"

#include <esp_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAG "Event"

ESP_EVENT_DEFINE_BASE(SYS_EVENT);

typedef struct
{
    char *key;
    char *value;
} event_t;

typedef enum
{
    SYS_EVENT_ID,
} event_id_t;

#define CAPACITY 50  // Size of the Hash Table

static unsigned long hash_function(char *key)
{
    unsigned long c = 0;
    for (int i = 0; i < strlen(key); i++)
        c += (unsigned long)key[i];

    return c % CAPACITY;
}

typedef struct Ht_item Ht_item;

// Define the Hash Table Item here
struct Ht_item
{
    char *key;
    char *value;
};

typedef struct LinkedList LinkedList;

// Define the Linkedlist here
struct LinkedList
{
    Ht_item *item;
    LinkedList *next;
};

typedef struct HashTable HashTable;

// Define the Hash Table here
struct HashTable
{
    // Contains an array of pointers
    // to items
    Ht_item **items;
    LinkedList **overflow_buckets;
    int size;
    int count;
};

static LinkedList *allocate_list()
{
    // Allocates memory for a Linkedlist pointer
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    return list;
}

static LinkedList *linkedlist_insert(LinkedList *list, Ht_item *item)
{
    // Inserts the item onto the Linked List
    if (!list)
    {
        LinkedList *head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    }

    else if (list->next == NULL)
    {
        LinkedList *node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList *temp = list;
    while (temp->next->next)
    {
        temp = temp->next;
    }

    LinkedList *node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;

    return list;
}

static Ht_item *linkedlist_remove(LinkedList *list)
{
    // Removes the head from the linked list
    // and returns the item of the popped element
    if (!list)
        return NULL;
    if (!list->next)
        return NULL;
    LinkedList *node = list->next;
    LinkedList *temp = list;
    temp->next = NULL;
    list = node;
    Ht_item *it = NULL;
    memcpy(temp->item, it, sizeof(Ht_item));
    free(temp->item->value);
    free(temp->item);
    free(temp);
    return it;
}

static void free_linkedlist(LinkedList *list)
{
    LinkedList *temp = list;
    while (list)
    {
        temp = list;
        list = list->next;
        free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}

static LinkedList **create_overflow_buckets(HashTable *table)
{
    // Create the overflow buckets; an array of linkedlists
    LinkedList **buckets = (LinkedList **)calloc(table->size, sizeof(LinkedList *));
    for (int i = 0; i < table->size; i++)
        buckets[i] = NULL;
    return buckets;
}

static void free_overflow_buckets(HashTable *table)
{
    // Free all the overflow bucket lists
    LinkedList **buckets = table->overflow_buckets;
    for (int i = 0; i < table->size; i++)
        free_linkedlist(buckets[i]);
    free(buckets);
}

static Ht_item *create_item(char *key, char *value)
{
    // Creates a pointer to a new hash table item
    Ht_item *item = (Ht_item *)malloc(sizeof(Ht_item));
    item->key = (char *)malloc(strlen(key) + 1);
    item->value = (char *)malloc(strlen(value) + 1);

    strcpy(item->key, key);
    strcpy(item->value, value);

    return item;
}

static HashTable *create_table(int size)
{
    // Creates a new HashTable
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item **)calloc(table->size, sizeof(Ht_item *));
    for (int i = 0; i < table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets(table);

    return table;
}

static void free_item(Ht_item *item)
{
    // Frees an item
    free(item->key);
    free(item->value);
    free(item);
}

static void free_table(HashTable *table)
{
    // Frees the table
    for (int i = 0; i < table->size; i++)
    {
        Ht_item *item = table->items[i];
        if (item != NULL)
            free_item(item);
    }

    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

static void handle_collision(HashTable *table, unsigned long index, Ht_item *item)
{
    LinkedList *head = table->overflow_buckets[index];

    if (head == NULL)
    {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else
    {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
}

static void ht_insert(HashTable *table, char *key, char *value)
{
    // Create the item
    Ht_item *item = create_item(key, value);

    // Compute the index
    unsigned long index = hash_function(key);

    Ht_item *current_item = table->items[index];

    if (current_item == NULL)
    {
        // Key does not exist.
        if (table->count == table->size)
        {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item(item);
            return;
        }

        // Insert directly
        table->items[index] = item;
        table->count++;
    }

    else
    {
        // Scenario 1: We only need to update value
        if (strcmp(current_item->key, key) == 0)
        {
            strcpy(table->items[index]->value, value);
            return;
        }

        else
        {
            // Scenario 2: Collision
            handle_collision(table, index, item);
            return;
        }
    }
}

static void *ht_search(HashTable *table, char *key)
{
    // Searches the key in the hashtable
    // and returns NULL if it doesn't exist
    int index = hash_function(key);
    Ht_item *item = table->items[index];
    LinkedList *head = table->overflow_buckets[index];

    // Ensure that we move to items which are not NULL
    while (item != NULL)
    {
        if (strcmp(item->key, key) == 0)
            return item->value;
        if (head == NULL)
            return NULL;
        item = head->item;
        head = head->next;
    }
    return NULL;
}

// static void print_search(HashTable *table, char *key)
// {
//     char *val;
//     if ((val = ht_search(table, key)) == NULL)
//     {
//         printf("%s does not exist\n", key);
//         return;
//     }
//     else
//     {
//         printf("Key:%s, Value:%s\n", key, val);
//     }
// }

static void print_table(HashTable *table)
{
    printf("\n-------------------\n");
    for (int i = 0; i < table->size; i++)
    {
        if (table->items[i])
        {
            printf("Index:%d, \"%s\":\"%s\"", i, table->items[i]->key, table->items[i]->value);
            if (table->overflow_buckets[i])
            {
                printf(" => Overflow Bucket => ");
                LinkedList *head = table->overflow_buckets[i];
                while (head)
                {
                    printf(", \"%s\":\"%s\"", table->items[i]->key, table->items[i]->value);
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}

static HashTable *table;
static esp_event_handler_instance_t listen_all_event;
static QueueHandle_t _lock_mutex;

typedef struct event_callback {
    char* key;
    callback_func_t cb;
    struct event_callback *next;
    struct event_callback *prev;
} event_callback_t;

static event_callback_t *callback_list = NULL;

static void listen_all_event_handler(void *event_handler_arg,
                                     esp_event_base_t event_base,
                                     int32_t event_id,
                                     void *event_data)
{
    event_t *ev = (event_t *)event_data;
    ESP_LOGI(TAG, "Event callback: {\"%s\": \"%s\"}", ev->key, ev->value);

    xSemaphoreTake(_lock_mutex, portMAX_DELAY);
    ht_insert(table, ev->key, ev->value);
    for(event_callback_t *ev_cb = callback_list; ev_cb != NULL; ev_cb = ev_cb->next) {
        if (strcmp(ev->key, ev_cb->key) == 0) {
            ev_cb->cb(ev->value);
        }
    }
    free(ev->key);
    free(ev->value);
    xSemaphoreGive(_lock_mutex);
}

int event_init(void)
{
    _lock_mutex = xQueueCreateMutex(1);
    table = create_table(CAPACITY);
    ESP_LOGI(TAG, "Event service start");
    esp_event_loop_create_default();

    esp_event_handler_instance_register(SYS_EVENT,
                                        SYS_EVENT_ID,
                                        &listen_all_event_handler,
                                        NULL,
                                        listen_all_event);

    return 0;
}

char *get_event(const char *key)
{
    xSemaphoreTake(_lock_mutex, portMAX_DELAY);
    event_t *ev = (event_t *)ht_search(table, key);
    xSemaphoreGive(_lock_mutex);

    return ev->value;
}

void set_event(const char *key, const char *value)
{
    event_t ev = {
        .key = strdup(key),
        .value = strdup(value),
    };

    ESP_LOGI(TAG, "Set event: {\"%s\": \"%s\"}", key, value);
    esp_event_post(SYS_EVENT, SYS_EVENT_ID, &ev, sizeof(event_t), portMAX_DELAY);
}

void show_event(void)
{
    print_table(table);
}

event_context register_event(const char *key, callback_func_t cb)
{
    event_callback_t *ev_cb = calloc(1, sizeof(event_callback_t));
    ev_cb->key = strdup(key);
    ev_cb->cb = cb;

    ESP_LOGI(TAG, "Registering %s event callback", key);

    xSemaphoreTake(_lock_mutex, portMAX_DELAY);
    if (callback_list == NULL) {
        callback_list = ev_cb;
    } else {
        ev_cb->next = callback_list;
        callback_list->prev = ev_cb;
        callback_list = ev_cb;
    }
    xSemaphoreGive(_lock_mutex);
    return (event_context)ev_cb;
}

void unregiter_event(event_context ctx) {
    event_callback_t *ev_cb = (event_callback_t *)ctx;
    xSemaphoreTake(_lock_mutex, portMAX_DELAY);

    if (ev_cb == callback_list) {
        free(ev_cb->key);
        free(ev_cb);
        xSemaphoreGive(_lock_mutex);
        return;
    }
    if (ev_cb->next != NULL) {
        ev_cb->next->prev = ev_cb->prev;
    }

    ev_cb->prev->next = ev_cb->next;

    free(ev_cb->key);
    free(ev_cb);
    xSemaphoreGive(_lock_mutex);
}
