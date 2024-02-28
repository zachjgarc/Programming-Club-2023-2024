#include <iostream>

struct Node {
  int data;
  Node *next;

  Node(int data) : data(data), next(nullptr){};
};

Node *head = nullptr;

void addNode(int data) {
  Node *newNode = new Node(data);

  if (head != nullptr) {
    newNode->next = head;
  }

  head = newNode;
}

void removeNode(int data) {
  Node *previous = nullptr;
  Node *current = head;

  while (current != nullptr) {
    if (current->data == data) {
      if (current == head) {
        head = current->next;
      } else {
        previous->next = current->next;
      }

      delete current;
      return;
    }

    previous = current;
    current = current->next;
  }

  std::cout << "Inputted value does not exist in the list.\n";
}

int findNode(int data) {
  Node *current = head;
  int count = 0;

  while (current != nullptr) {
    if (current->data == data) {
      return count;
    }

    current = current -> next;
    ++count;
  }

  return -1;
}

void insertNode(int data, int position) {
  Node *newNode = new Node(data);

  if (position == 0) {
    newNode->next = head;
    head = newNode;
  } else {
    Node* previous = nullptr;
    Node* current = head;
    
    while (current != nullptr) {
      if (position == 0) break;

      previous = current;
      current = current->next;
      --position;
    }

    previous->next = newNode;
    newNode->next = current;
  }
}

void printList() {
  Node *current = head;
  while (current != nullptr) {
    std::cout << current->data << " -> ";
    current = current->next;
  }
  std::cout << "nullptr" << std::endl;
}

void printMenu() {
  std::cout << "You have the following options:\n";
  std::cout << "\t1. Add a node to the list.\n";
  std::cout << "\t2. Delete a node from the list.\n";
  std::cout << "\t3. Search for a node in the list.\n";
  std::cout << "\t4. Insert a node to the list.\n";
  std::cout << "\t5. Print the list.\n";
  std::cout << "\t6. Quit.\n";
  return;
}

int main() {
  int option = -1;
  while (option != 6) {
    printMenu();
    std::cin >> option;
    int inputValue;

    switch (option) {
    case 1:
      // add operation
      std::cout << "What value would you like to add?\n";
      std::cin >> inputValue;
      addNode(inputValue);
      break;

    case 2:
      // delete operation
      std::cout << "What value would you like to remove?\n";
      std::cin >> inputValue;
      removeNode(inputValue);
      break;

    case 3: {
      // search operation
      std::cout << "What value would you like to search for?\n";
      std::cin >> inputValue;
      int index = findNode(inputValue);
      std::cout << "The value " << inputValue << " exists at index " << index << std::endl;
      break;
    }

    case 4: {
      // insert operation
      std::cout << "What value would you like to add?\n";
      std::cin >> inputValue;
      int inputPosition;
      std::cout << "What position would you like to insert it at?\n";
      std::cin >> inputPosition;
      insertNode(inputValue, inputPosition);
      break;
    }

    case 5:
      printList();
      break;

    case 6:
      break;
    }
  }
}
