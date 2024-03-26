#include "DoubleLinkedList.h" // Assuming this header file contains the Node structure and function prototypes

int main() {
    Node *head = NULL; // Initialize an empty linked list

    // Insert some elements into the linked list
    
     for(int i = 0; i < 5; i++)
           append(&head, i);
    
    // Print the initial list
    printf("Initial list: ");
    printList(head);
    printf("\n\n");

    // Insert a new node with data 4 at the front of the list
    insertFront(&head, 4);
    printf("After inserting 4 at the front: ");
    printList(head);
    printf("\n\n");

    // Insert a new node with data 5 after the node containing data 2
    Node *nodeContaining2 = head;
    insertAfter(nodeContaining2, 2, 5);
    printf("After inserting 5 after the node containing 2: ");
    printList(head);
    printf("\n\n");

    // Insert a new node with data 6 before the node containing data 3
    Node *nodeContaining3 = head;
    insertBefore(&nodeContaining3, 2, 6);
    printf("After inserting 6 before the node containing 2: ");
    printList(head);
    printf("\n\n");
    // Delete the node containing data 2
    deleteNode(&head, nodeContaining2);
    printf("After deleting the node containing 2: ");
    printList(head);
    printf("\n\n");


    append(&head, 10);
    printf("After appending 10 at the end: ");
    printList(head);
    printf("\n\n");

    // deleteNodeAfter(&head, 3);
    // printList(head);
    // printf("\n\n");

    deleteNodeBefore(&head, 3);
    printList(head);
    printf("\n\n");

    // Free the memory allocated for the linked list
    DeleteList(&head);

    return 0;
}
