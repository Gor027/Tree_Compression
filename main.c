// First Name: Gor
// Last Name: Stepanyan
// Nr. Indeksu: 404865
// Email: gor.stepanyan027@gmail.com
// USOS Email: gs404865@students.mimuw.edu.pl

/*
 * The "Tree Compression" is a project to compress a binary tree given the preorder traversal of it.
 * The aim is to replace all of the duplicate subtrees with a node in a certain value in it.
 * The value is determined by the order of the first occurrence of of the original subtree(N)
 * and the order of the duplicate subtree(K). The replaced node value is K-N.
 * The order is the row number of the printed node value starting from 1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//The binary tree node structure
struct Node {
    int value; //Value of the node given in input
    int order; //Order is necessary for replacing duplicate subtree with certain value
    struct Node *original; //Null when subtree is unique, otherwise is the reference to original subtree
    struct Node *left; //Left Subtree
    struct Node *right; //Right Subtree
    struct Node *parent; //Reference to parent node
};

//Gets each row int value
int get_row() {
    int value = 0;

    int sign;

    while ((sign = getchar()) != '\n' && sign != EOF) {
        value = value * 10 + sign - '0';
    }

    if (sign == EOF) {
        return -1;
    }

    return value;
}

//Gets the binary tree using get_row() for identifying the value for each node
struct Node **get_tree(struct Node **root, int *leavesCount) {
    int row_number = 0;

    struct Node **leaves = NULL;
    bool left = false;

    struct Node *p = NULL;

    int value;
    while ((value = get_row()) != -1) {
        row_number++;
        if (value == 0) {
            if (!left) {
                left = true;
            } else {
                do {
                    if (p->left == NULL && p->right == NULL) {
                        leaves = (struct Node **) realloc(leaves, ++*leavesCount * sizeof(struct Node *));
                        leaves[*leavesCount - 1] = p;
                    }
                    p = p->parent;
                } while (p != NULL && p->right != NULL);
            }
            continue;
        }

        if (p == NULL) {
            p = malloc(sizeof(struct Node));
            p->parent = NULL;
            *root = p;
        } else if (p->left == NULL && !left) {
            p->left = malloc(sizeof(struct Node));
            p->left->parent = p;
            p = p->left;
        } else {
            p->right = malloc(sizeof(struct Node));
            p->right->parent = p;
            p = p->right;
        }

        left = false;

        p->left = NULL;
        p->right = NULL;
        p->value = value;
        p->original = NULL;
        p->order = row_number;
    }

    free(p);
    return leaves;
}

//Prints the final result tree
int print_tree(struct Node *p, int order) {
    if (p == NULL) {
        printf("0\n");
        return order;
    }

    p->order = order;

    if (p->original != NULL) {
        printf("%d\n", p->original->order - p->order);
        return order;
    } else {
        printf("%d\n", p->value);
        order = print_tree(p->left, order + 1);
        order = print_tree(p->right, order + 1);
    }

    return order;
}

//Check subtrees with given roots are identical
bool isSubtreeIdentical(struct Node *original, struct Node *duplicate) {
    if (original == NULL) {
        return duplicate == NULL;
    } else if (duplicate == NULL) {
        return original == NULL;
    }

    if (original->value != duplicate->value) {
        return false;
    }

    return isSubtreeIdentical(original->left, duplicate->left) && isSubtreeIdentical(original->right, duplicate->right);
}

//Traverse the from the leaves to mark duplicate subtrees
void mark_dead_leaves(struct Node **leaves, int leavesCount) {
    int i = 0;
    int j = 1;

    struct Node *original;
    struct Node *duplicate;

    while (j < leavesCount) {
        if (leaves[i]->value != leaves[j]->value) {
            i = j;
            j++;
            continue;
        }

        for (int i1 = i; i1 < j; ++i1) {

            duplicate = leaves[j];
            original = leaves[i1];

            if(duplicate->original == NULL) {
                duplicate->original = original;
            }

            while (original->parent != duplicate->parent && original->parent->value == duplicate->parent->value) {
                original = original->parent;
                duplicate = duplicate->parent;

                if (!isSubtreeIdentical(original, duplicate)) {
                    break;
                }

                if(duplicate->original == NULL) {
                    duplicate->original = original;
                }
            }
        }

        j++;
    }
}

//Used in sort_leaves() as utility function for mergeing arrays in a sorted order
void merge(struct Node **arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    struct Node *L[n1];
    struct Node *R[n2];

    for (int i = 0; i < n1; i++) {
        L[i] = arr[l + i];
    }

    for (int j = 0; j < n2; j++) {
        R[j] = arr[m + 1 + j];
    }

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i]->value < R[j]->value) {
            arr[k] = L[i];
            i++;
        } else if (L[i]->value > R[j]->value) {
            arr[k] = R[j];
            j++;
        } else {
            if (L[i]->order <= R[j]->order) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

}

//Function to sort all of the leaf nodes for traversing bottom_up in the future
void sort_leaves(struct Node **arr, int l, int r) {
    if (l >= r)
        return;

    int mid = l + (r - l) / 2;
    sort_leaves(arr, l, mid);
    sort_leaves(arr, mid + 1, r);

    merge(arr, l, mid, r);
}

//Function to avoid memory leaks and valgrind
void freeTree(struct Node *root) {
    if (root == NULL)
        return;

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main() {
    struct Node *root = NULL;

    int leavesCount = 0;

    struct Node **leaves = get_tree(&root, &leavesCount);

    sort_leaves(leaves, 0, leavesCount - 1);

    mark_dead_leaves(leaves, leavesCount);

    print_tree(root, 1);

    free(leaves);
    freeTree(root);

    return 0;
}
