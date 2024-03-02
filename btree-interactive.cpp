#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

struct BTree {
  int val;
  BTree *left;
  BTree *right;
};

struct Zipper {
  std::vector<bool> *path; // left = 0, right = 1
  int idx; // -1 if non-cursor branch or leaf; equal to path length if cursor
};

Zipper try_zip(Zipper zip, bool branch) {
  int idx = -1;

  if (zip.idx != -1 && zip.idx < zip.path->size() &&
      zip.path->at(zip.idx) == branch) {
    idx = zip.idx + 1;
  }

  return {zip.path, idx};
}

// Not calling it depth because I'm just like that
int calc_height(BTree *tree) {
  if (tree == nullptr) {
    return 0;
  }
  return std::max(calc_height(tree->left), calc_height(tree->right)) + 1;
}

std::vector<int> exp_sum_cache{0};

int exp_sum(int n) {
  if (n < exp_sum_cache.size()) {
    return exp_sum_cache.at(n);
  }

  int sum = exp_sum_cache.back();
  int exp = 1 << exp_sum_cache.size();

  while (exp_sum_cache.size() <= n) {
    sum += exp;
    exp_sum_cache.push_back(sum);
    exp *= 2;
  }

  return sum;
}

void set_at_fill(std::string *into, int at, char c) {
  int fill = at - into->size() + 1;
  if (fill > 0) {
    into->append(fill, ' ');
  }

  into->at(at) = c;
}

void append_at_fill(std::string *into, int at, std::string *str) {
  int fill = at - into->size();
  if (fill > 0) {
    into->append(fill, ' ');
  }

  into->append(*str);
  into->append(1, ' '); // A little padding does a computer program good
}

int MAX_DIGITS = 3;

void to_string_rec(BTree *tree, Zipper cursor, int height, int depth,
                   std::vector<std::string> *lines, int curr_line, bool branch,
                   int nth_in_row, int prev_pos) {
  if (tree == nullptr) {
    return;
  }

  int depth_rev = height - depth - 1;

  int offset = exp_sum(depth_rev);
  int val_pos =
      1 + offset + (MAX_DIGITS + exp_sum(depth_rev + 1) - 1) * nth_in_row;

  char slash;
  int shift;

  if (branch == 0) {
    slash = '/';
    shift = -1;
  } else {
    slash = '\\';
    shift = 1;
  }

  int center = prev_pos + 1;
  int pos = val_pos + 1;

  if (depth == 0) {
    // Root; no branch
  } else if (depth_rev == 0) {
    // Only use one slash
    set_at_fill(&lines->at(curr_line), center + shift, slash);
    curr_line++;
  } else if (depth_rev == 1) {
    // Use only slashes for branch
    for (int i = 1; i <= 3; i++) {
      set_at_fill(&lines->at(curr_line), center + shift * i, slash);
      curr_line++;
    }
  } else {
    // Use slashes with funky dash thingy for branch
    set_at_fill(&lines->at(curr_line), center + shift, slash);
    set_at_fill(&lines->at(curr_line + 1), center + shift * 2, slash);
    set_at_fill(&lines->at(curr_line + 1), pos - shift * 2, '+');
    set_at_fill(&lines->at(curr_line + 2), pos - shift, slash);

    int a = center + shift * 3;
    int b = pos - shift * 3;

    int from = std::min(a, b);
    int to = std::max(a, b);

    for (int i = from; i <= to; i++) {
      set_at_fill(&lines->at(curr_line + 1), i, '-');
    }

    curr_line += 3;
  }

  std::string val = std::to_string(tree->val);
  if (val.size() == 1) {
    val.insert(0, 1, ' '); // Pad value if single digit
  }

  append_at_fill(&lines->at(curr_line), val_pos, &val);

  if (cursor.idx == cursor.path->size()) {
    set_at_fill(&lines->at(curr_line), val_pos - 1, '[');
    set_at_fill(&lines->at(curr_line), val_pos + MAX_DIGITS, ']');
  }

  curr_line++;

  to_string_rec(tree->left, try_zip(cursor, 0), height, depth + 1, lines,
                curr_line, 0, nth_in_row * 2, val_pos);
  to_string_rec(tree->right, try_zip(cursor, 1), height, depth + 1, lines,
                curr_line, 1, nth_in_row * 2 + 1, val_pos);
}

void to_string_pretty(BTree *root, Zipper cursor, std::string *out) {
  out->clear();

  if (root == nullptr) {
    out->append("(empty tree)\n");
    return;
  }

  int height = calc_height(root);
  int line_count = height == 1 ? 1 : height * 4 - 5;
  std::vector<std::string> lines;
  for (int i = 0; i < line_count; i++) {
    lines.push_back(std::string());
  }

  to_string_rec(root, cursor, height, 0, &lines, 0, 0, 0, 0);

  for (int i = 0; i < line_count; i++) {
    out->append(lines.at(i));
    out->append(1, '\n');
  }

  return;
}

bool at_cursor(Zipper cursor) { return cursor.idx == cursor.path->size(); }

// Returns -1 if no next branch
int get_next_branch(Zipper cursor) {
  if (cursor.idx == -1 || at_cursor(cursor)) {
    return -1;
  }
  return cursor.path->at(cursor.idx);
}

BTree *get_cursor(BTree *tree, Zipper cursor) {
  while (tree != nullptr) {
    if (at_cursor(cursor)) {
      return tree;
    }

    int branch = get_next_branch(cursor);

    if (branch == 0) {
      tree = tree->left;
    } else if (branch == 1) {
      tree = tree->right;
    } else {
      return nullptr;
    }

    cursor.idx++;
  }

  return nullptr;
}

// -1 if root
int get_cursor_branch(Zipper cursor) {
  if (cursor.path->size() == 0) {
    return -1;
  }

  return cursor.path->back();
}

void cursor_branch(BTree *root, Zipper cursor, bool branch) {
  BTree *curr = get_cursor(root, cursor);

  if (curr == nullptr) {
    return;
  }

  if (branch == 0 && curr->left != nullptr) {
    cursor.path->push_back(0);
  } else if (branch == 1 && curr->right != nullptr) {
    cursor.path->push_back(1);
  }
}

void cursor_parent(Zipper cursor) {
  if (cursor.path->size() > 0) {
    cursor.path->pop_back();
  }
}

void insert_node(BTree *tree, bool branch) {
  if (branch == 0) {
    BTree *node = new BTree{0, tree->left, nullptr};
    tree->left = node;
  } else {
    BTree *node = new BTree{0, nullptr, tree->right};
    tree->right = node;
  }
}

// Returns true if successful, false if unsuccessful
// Returns new tree, null if unsuccessful
std::tuple<bool, BTree *> delete_node(BTree *tree) {
  if (tree == nullptr) {
    return {true, nullptr};
  }

  if (tree->left != nullptr && tree->right != nullptr) {
    return {false, tree};
  }

  BTree *child = nullptr;

  if (tree->left != nullptr) {
    child = tree->left;
  } else if (tree->right != nullptr) {
    child = tree->right;
  }

  delete tree;
  return {true, child};
}

void delete_tree(BTree *tree) {
  if (tree == nullptr) {
    return;
  }

  delete_tree(tree->left);
  delete_tree(tree->right);
  delete tree;
}

// My beloved.
void invert_tree(BTree *tree) {
  if (tree == nullptr) {
    return;
  }

  BTree *left = tree->left;
  BTree *right = tree->right;
  invert_tree(left);
  invert_tree(right);
  tree->left = right;
  tree->right = left;
}

void clear_line() { std::cout << "\33[2K\r"; }

void clear_screen() { std::cout << "\033[2J\033[1;1H"; }

std::string ctrl_fmt;
std::string ctrl_list[] = {
    "[W] Cursor Parent",     "[A] Cursor Left Child",  "[D] Cursor Right Child",
    "[S] Insert Left Child", "[F] Insert Right Child", "[Z] Change Node Value",
    "[X] Delete Node",       "[C] Delete Subtree",     "[V] Invert Subtree",
};

void init_controls(int ctrl_width, int line_width) {
  int curr_line_width = 0;

  for (std::string ctrl : ctrl_list) {
    curr_line_width += ctrl_width;

    if (curr_line_width > line_width) {
      ctrl_fmt.append(1, '\n');
      curr_line_width = ctrl_width;
    }

    ctrl_fmt.append(ctrl);
    ctrl_fmt.append(ctrl_width - ctrl.size(), ' ');
  }
}

void loop() {
  BTree *root = nullptr;
  // BTree *a = new BTree{4, new BTree{8, 0, 0}, new BTree{9, 0, 0}};
  // BTree *b = new BTree{5, new BTree{10, 0, 0}, new BTree{11, 0, 0}};
  // BTree *c = new BTree{6, new BTree{12, 0, 0}, new BTree{13, 0, 0}};
  // BTree *d = new BTree{7, new BTree{14, 0, 0}, new BTree{15, 0, 0}};
  // BTree *root = new BTree{1, new BTree{2, a, b}, new BTree{3, c, d}};

  std::vector<bool> zipper_path;
  Zipper cursor = {&zipper_path, 0};

  bool foolish = false;

  std::string buffer;

  while (true) {
    to_string_pretty(root, cursor, &buffer);
    clear_screen();

    std::cout << buffer << '\n';

    if (root != nullptr) {
      std::cout << "Depth: " << cursor.path->size() << "\n\n";
    }

    std::cout << ctrl_fmt << '\n';

    if (foolish) {
      std::cout << "\nYou may only delete nodes with only one child.";
      foolish = false;
    }

    std::cout << "\n:";

    char action;
    std::cin >> action;
    action = tolower(action);

    int val, branch;
    std::tuple<bool, BTree *> result;

    switch (action) {
    case 'w':
      cursor_parent(cursor);
      break;

    case 'a':
      cursor_branch(root, cursor, 0);
      break;

    case 'd':
      cursor_branch(root, cursor, 1);
      break;

    case 's':
      if (root == nullptr) {
        root = new BTree{0, nullptr, nullptr};
      } else {
        insert_node(get_cursor(root, cursor), 0);
        cursor_branch(root, cursor, 0);
      }

      break;

    case 'f':
      if (root == nullptr) {
        root = new BTree{0, nullptr, nullptr};
      } else {
        insert_node(get_cursor(root, cursor), 1);
        cursor_branch(root, cursor, 1);
      }

      break;

    case 'z':
      std::cout << "Enter an integer between 0 and 999.\n";
      while (true) {
        std::cout << ':';
        std::cin >> val;

        if (val >= 0 && val <= 999) {
          break;
        } else {
          std::cout << "Value must be an integer between 0 and 999.\n";
        }
      }

      if (root == nullptr) {
        root = new BTree{val, nullptr, nullptr};
      } else {
        get_cursor(root, cursor)->val = val;
      }

      break;

    case 'x':
      branch = get_cursor_branch(cursor);
      result = delete_node(get_cursor(root, cursor));

      if (std::get<0>(result)) {
        cursor_parent(cursor);

        if (branch == 0) {
          get_cursor(root, cursor)->left = std::get<1>(result);
        } else if (branch == 1) {
          get_cursor(root, cursor)->right = std::get<1>(result);
        } else {
          root = std::get<1>(result);
        }
      } else {
        foolish = true;
      }

      break;

    case 'c':
      branch = get_cursor_branch(cursor);
      delete_tree(get_cursor(root, cursor));
      cursor_parent(cursor);

      if (branch == 0) {
        get_cursor(root, cursor)->left = nullptr;
      } else if (branch == 1) {
        get_cursor(root, cursor)->right = nullptr;
      } else {
        root = nullptr;
      }

      break;

    case 'v':
      invert_tree(get_cursor(root, cursor));
      break;

    case ' ':
      break;

    default:
      std::cout << "Got invalid action; quitting\n";
      delete_tree(root);
      return;
    }
  }
}

int main() {
  init_controls(26, 80);
  loop();
}
