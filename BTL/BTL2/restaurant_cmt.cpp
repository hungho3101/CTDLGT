#include "main.h"

int MAXSIZE;
int line = 0;
int numOrder = 0;

class HuffNode;
class BSTG;
// Tạo cấu trúc dữ liệu cho node trong MinHeap S
class HeapNode
{
public:
	int label;	// Nhãn
	int num;	// Số lượng khách hàng
	int order;	// Thứ tự thêm vào heap (order nhỏ có nghĩa node Heap được thêm vào sớm hơn)
	int order2; // Thời gian sử dụng khu (order2 nhỏ có nghĩa khu chưa được sử dụng lâu hơn)
	HeapNode(int label, int num) : label(label), num(num), order(0), order2(0) {}
};

void LAPSE(string name);
void KOKUSEN();
void KEITEIKEN(int num);
void HAND();
void LIMITLESS(int num);
void CLEAVE(int num);
void deleteAll();

// Khai báo các biến
// vector<pair<HuffNode *, int>> custom;		// Lưu danh sách tất cả khách hàng theo thứ tự
vector<pair<HuffNode *, int>> customCaesar; // Lưu danh sách khách hàng mã hóa Caesar theo thứ tự
vector<pair<HuffNode *, int>> custom;
vector<vector<pair<HuffNode *, int>>> Gcus; // Danh sách lưu thứ tự khách hàng khu vực i trong G
vector<vector<pair<HuffNode *, int>>> Scus; // Danh sách lưu thứ tự khách hàng khu vực i trong S
vector<BSTG *> GcusTree;					// Danh sách cây BTSG của khu vực i trong G
vector<HeapNode *> SHeapNode;				// Danh sách quản lý node trong MinHeap S

void simulate(string filename)
{
	ifstream ss(filename);
	string str, maxsize, name, energy, num;
	while (ss >> str)
	{
		line += 1;
		cout << "Line: " << line << endl;
		if (str == "MAXSIZE") // MAXSIZE <NUM>
		{
			ss >> maxsize;
			MAXSIZE = stoi(maxsize);
			GcusTree.resize(MAXSIZE + 1, nullptr);
			Gcus.resize(MAXSIZE + 1);
			Scus.resize(MAXSIZE + 1);
			SHeapNode.resize(MAXSIZE + 1);
			for (int i = 1; i <= MAXSIZE; i++)
			{
				SHeapNode[i] = new HeapNode(i, 0);
			}
			// std::cout << "MAXSIZE " << MAXSIZE << endl;
		}
		else if (str == "LAPSE") // LAPSE <NAME>
		{
			ss >> name;
			// std::cout << "LAPSE : LINE " << line << endl;
			LAPSE(name);
		}
		else if (str == "KOKUSEN")
		{
			// std::cout << "KOKUSEN LINE: " << line << endl;
			KOKUSEN();
		}
		else if (str == "KEITEIKEN") // KEITEIKEN <NUM>
		{
			ss >> num;
			// std::cout << "KEITEIKEN LINE: " << line << endl;
			KEITEIKEN(stoi(num));
		}
		else if (str == "HAND") // HAND
		{
			// std::cout << "HAND : LINE " << line << endl;
			HAND();
		}
		else if (str == "LIMITLESS") // LIMITLESS <NUM>
		{
			ss >> num;
			// std::cout << "LIMITLESS LINE: " << line << endl;
			LIMITLESS(stoi(num));
		}
		else if (str == "CLEAVE") // CLEAVE <NUM>
		{
			ss >> num;
			// std::cout << "CLEAVE LINE: " << line << endl;
			CLEAVE(stoi(num));
		}
	}

	deleteAll();
	// std::cout << "END";
}

/*---------------------------------------------------*/
/*						LAPSE						 */
/*---------------------------------------------------*/

// Kiểm tra xem có tối thiểu 3 ký tự khác nhau trong chuỗi không
bool hasThreeDistinctChars(const string &str)
{
	set<char> distinct_chars;
	for (char c : str)
	{
		distinct_chars.insert(c);
		if (distinct_chars.size() >= 3)
		{
			return true;
		}
	}
	return false;
}

// Hàm để thực hiện việc mã hóa và in ra kết quả
string MaHoaCaesar(string s)
{
	// Sắp xếp các ký tự theo tần suất tăng dần
	vector<pair<char, int>> freq_vec;
	for (char c : s)
	{
		bool found = false;
		for (auto &p : freq_vec)
		{
			if (p.first == c)
			{
				p.second++;
				found = true;
				break;
			}
		}
		if (!found)
			freq_vec.push_back({c, 1});
	}

	sort(freq_vec.begin(), freq_vec.end(),
		 [](const pair<char, int> &a, const pair<char, int> &b)
		 {
			 return a.second < b.second;
		 });

	string sorted_str;
	for (const auto &p : freq_vec)
	{
		sorted_str += string(p.second, p.first);
	}
	s = sorted_str;
	// std::cout << "Sap xep: " << s << endl;

	// Mã hóa chuỗi đã được sắp xếp
	for (char &c : s)
	{
		int freq = 0;
		for (auto &p : freq_vec)
		{
			if (p.first == c)
			{
				freq = p.second;
				break;
			}
		}
		char base = isupper(c) ? 'A' : 'a';
		c = (c - base + freq) % 26 + base;
	}

	// Sắp xếp các ký tự theo tần suất tăng dần
	vector<pair<char, int>> freq_vec2;
	for (char c : s)
	{
		bool found = false;
		for (auto &p : freq_vec2)
		{
			if (p.first == c)
			{
				p.second++;
				found = true;
				break;
			}
		}
		if (!found)
			freq_vec2.push_back({c, 1});
	}

	// abcd...ABCD
	std::sort(s.begin(), s.end(), [](char a, char b)
			  {
	if (std::islower(a) && std::isupper(b))
	    return true;
	if (std::isupper(a) && std::islower(b))
	    return false;
	return a < b; });

	sort(freq_vec2.begin(), freq_vec2.end(),
		 [](const pair<char, int> &a, const pair<char, int> &b)
		 {
			 if (a.second == b.second)
			 {
				 if (std::islower(a.first) && std::isupper(b.first))
					 return true;
				 if (std::isupper(a.first) && std::islower(b.first))
					 return false;
				 return a.first < b.first;
			 }
			 else
				 return a.second < b.second;
		 });

	string sorted_str2;
	for (const auto &p : freq_vec2)
		sorted_str2 += string(p.second, p.first);
	s = sorted_str2;
	// std::cout << "Sort ma hoa: " << s << endl;
	return s;
}

// Mã hóa tên khách hàng
string MaHoaTen(string name)
{
	vector<pair<char, int>> freq_vec;
	for (char c : name)
	{
		bool found = false;
		for (auto &p : freq_vec)
		{
			if (p.first == c)
			{
				p.second++;
				found = true;
				break;
			}
		}
		if (!found)
			freq_vec.push_back({c, 1});
	}

	string s;

	for (char c : name)
	{
		int freq = 0;
		for (auto &p : freq_vec)
		{
			if (p.first == c)
			{
				freq = p.second;
				break;
			}
		}
		char base = isupper(c) ? 'A' : 'a';
		c = (c - base + freq) % 26 + base;
		s += c;
	}
	return s;
}

// Lớp cơ sở trừu tượng cho nút cây Huffman
class HuffNode
{
public:
	char it;	  // Giá trị char
	int wgt;	  // Trọng số
	HuffNode *lc; // Nút con bên trái
	HuffNode *rc; // Nút con bên phải
	int order;	  // Thứ tự

public:
	~HuffNode() {}						// Hàm hủy cơ sở
	HuffNode(const char &val, int freq) // Hàm tạo
	{
		it = val;
		wgt = freq;
		lc = nullptr;
		rc = nullptr;
		order = numOrder++;
	}
	HuffNode(HuffNode *l, HuffNode *r) // Hàm tạo
	{
		it = '\0';
		wgt = l->weight() + r->weight(); // Tính trọng số
		lc = l;
		rc = r;
		order = numOrder++;
	}
	char getChar() { return it; }	// Trả về giá trị char
	int weight() { return wgt; }	// Trả về trọng số
	HuffNode *left() { return lc; } // Trả về nút con bên trái
	void setLeft(HuffNode *b)		// Đặt nút con bên trái
	{
		lc = b;
	}
	HuffNode *right() { return rc; } // Trả về nút con bên phải
	void setRight(HuffNode *b)		 // Đặt nút con bên phải
	{
		rc = b;
	}
};

// Trả về chiều cao HuffNode bất kỳ
int getHeight(HuffNode *node)
{
	if (node == nullptr)
	{
		return 0;
	}
	int leftHeight = getHeight(node->lc);
	int rightHeight = getHeight(node->rc);
	return std::max(leftHeight, rightHeight) + 1;
}

// Xoay phải
HuffNode *rightRotate(HuffNode *y)
{
	HuffNode *x = y->lc;
	HuffNode *T2 = x->rc;

	// Thực hiện xoay
	x->rc = y;
	y->lc = T2;

	// Trả về nút mới
	return x;
}

// Xoay trái
HuffNode *leftRotate(HuffNode *x)
{
	HuffNode *y = x->rc;
	HuffNode *T2 = y->lc;

	// Thực hiện xoay
	y->lc = x;
	x->rc = T2;

	// Trả về nút mới
	return y;
}

// HuffNode *balpre(HuffNode *root, int &count);

// Lấy cân bằng
int getBalance(HuffNode *N)
{
	if (N == nullptr)
		return 0;
	return getHeight(N->lc) - getHeight(N->rc);
}

// void printTree(HuffNode *root, string prefix = " ");
void printTree(HuffNode *root);

// Cân bằng cây Huffman
HuffNode *balanceHuffTree(HuffNode *root)
{
	if (root == nullptr || getHeight(root) < 3)
		return root;

	// Kiểm tra xem nút này có bị mất cân bằng không
	int balance = getBalance(root);
	// Nếu nút này mất cân bằng, thì có 4 trường hợp

	// Trường hợp trái trái
	if (balance > 1 && getBalance(root->left()) >= 0)
	{
		root = rightRotate(root);
		// cout << "TT\n";
	}

	// Trường hợp phải phải
	else if (balance < -1 && getBalance(root->right()) <= 0)
	{
		root = leftRotate(root);
		// cout << "PP\n";
	}

	// Trường hợp trái phải
	else if (balance > 1 && getBalance(root->lc) < 0)
	{
		root->lc = leftRotate(root->lc);
		root = rightRotate(root);
		// cout << "TP\n";
	}

	// Trường hợp phải trái
	else if (balance < -1 && getBalance(root->rc) > 0)
	{
		root->rc = rightRotate(root->rc);
		root = leftRotate(root);
		// cout << "PT\n";
	}
	return root;
}

// Cân bằng cây Huffman theo thứ tự trước
HuffNode *balpre(HuffNode *root, int &count)
{
	if (root == nullptr || count == 1)
		return root;
	if (getBalance(root) > 1 || getBalance(root) < -1)
	{
		// if (count >= 3)
		// {
		// 	break;
		// }
		// cout << "+++++++++++++++++++++++++++++++++++\n";
		root = balanceHuffTree(root);
		// printTree(root);
		// cout << "+++++++++++++++++++++++++++++++++++\n";
		count++;
	}

	root->lc = balpre(root->lc, count);
	root->rc = balpre(root->rc, count);
	return root;
}

// // In cây Huffman
// void printTree(HuffNode *root, string prefix)
// {
// 	if (root == nullptr)
// 	{
// 		return;
// 	}

// 	// Nếu nút hiện tại là nút lá, in ký tự và trọng số của nó
// 	if (getHeight(root) == 1)
// 	{
// 		std::cout << prefix << "Leaf: " << root->getChar() << " " << root->weight() << "\n";
// 	}
// 	else
// 	{
// 		// In trọng số của nút nội bộ
// 		std::cout << prefix << "Internal: " << root->weight() << "\n";
// 	}

// 	// Duyệt và in các nút con
// 	printTree(root->left(), prefix + "--");
// 	printTree(root->right(), prefix + "--");
// }

// In cây Huffman
void printTree(HuffNode *root)
{
	if (root == nullptr)
	{
		return;
	}

	// Nếu nút hiện tại là nút lá, in ký tự và trọng số của nó
	if (getHeight(root) == 1)
	{
		std::cout << "[" << root->wgt << "," << root->it << "]";
	}
	else
	{
		// In trọng số của nút nội bộ
		std::cout << "[" << root->wgt << "]";
	}

	// Duyệt và in các nút con
	if (root->lc != nullptr || root->rc != nullptr)
	{
		std::cout << "(";
		printTree(root->left());
		std::cout << ",";
		printTree(root->right());
		std::cout << ")";
	}
}

// Xây dựng một cây Huffman từ một tập hợp các tần số
// So sánh hai nút cây Huffman
struct compare
{
	bool operator()(HuffNode *l, HuffNode *r)
	{
		if (l->weight() == r->weight())
		{
			return l->order > r->order; // Use the order of insertion as the second comparison criterion
		}
		return l->weight() > r->weight();
	}
};

// Xây dựng cây Huffman từ một tập hợp các tần số
HuffNode *buildHuff(vector<pair<char, int>> &freqs)
{
	priority_queue<HuffNode *, vector<HuffNode *>, compare> forest;
	// Tạo một nút lá cho mỗi tần số và thêm vào rừng
	for (const auto &freq : freqs)
	{
		forest.push(new HuffNode(freq.first, freq.second));
		// std::cout << freq.first << " " << freq.second << endl;
	}

	// // In ra thứ tự các nút lá thêm vào rừng
	// std::cout << "Thu tu them vao Huffman: ";
	// priority_queue<HuffNode *, vector<HuffNode *>, compare> forest1;
	// // Tạo một nút lá cho mỗi tần số và thêm vào rừng
	// for (const auto &freq1 : freqs)
	// {
	// 	forest1.push(new HuffNode(freq1.first, freq1.second));
	// 	// std::cout << freq.first << " " << freq.second << endl;
	// }
	// while (!forest1.empty())
	// {
	// 	std::cout << forest1.top()->getChar() << forest1.top()->wgt << " ";
	// 	forest1.pop();
	// }
	// std::cout << endl;

	// Xây dựng cây Huffman
	while (forest.size() > 1)
	{
		HuffNode *temp1 = forest.top();
		forest.pop();

		HuffNode *temp2 = forest.top();
		forest.pop();

		// Tạo một nút nội bộ mới với hai nút con là temp1 và temp2
		// Trọng số của nút nội bộ này là tổng trọng số của hai nút con
		HuffNode *temp = new HuffNode(temp1, temp2);
		// temp = preOrder(temp);
		for (int i = 0; i < 3; i++)
		{
			int count = 0;
			// printTree(temp);
			temp = balpre(temp, count);
		}

		temp->order = numOrder++;
		// cout << "**********************************\n";
		// printTree(temp);
		// cout << "**********************************\n";
		forest.push(temp);
		// printTree(temp);
	}
	return forest.top();
}

// Hàm tìm đường đi đến nút có giá trị cần tìm
string findPath(HuffNode *root, char value, string path = "")
{
	// Nếu nút hiện tại là null, trả về chuỗi rỗng
	if (root == nullptr)
	{
		return "";
	}

	// Nếu nút hiện tại có giá trị cần tìm, trả về đường đi hiện tại
	if (root->getChar() == value)
	{
		return path;
	}

	// Tìm kiếm trong nút con bên trái và bên phải
	string leftPath = findPath(root->left(), value, path + "0");
	string rightPath = findPath(root->right(), value, path + "1");

	// Nếu tìm thấy trong nút con bên trái, trả về đường đi bên trái
	if (!leftPath.empty())
	{
		return leftPath;
	}

	// Nếu tìm thấy trong nút con bên phải, trả về đường đi bên phải
	if (!rightPath.empty())
	{
		return rightPath;
	}

	// Nếu không tìm thấy, trả về chuỗi rỗng
	return "";
}

// Hàm chuyển đổi chuỗi nhị phân sang thập phân
int binaryToDecimal(string binary)
{
	int decimal = 0;
	int base = 1;
	int len = binary.length();

	// Lấy tối đa 10 ký tự từ chuỗi nhị phân từ phải sang trái
	int start = max(0, len - 10);
	for (int i = start; i < len; i++)
	{
		if (binary[i] == '1')
		{
			decimal += base;
		}
		base = base * 2;
	}

	return decimal;
}

// Tạo cấu trúc dữ liệu cho cây BST G
class BSTG
{
public:
	HuffNode *data;
	int key;
	BSTG *left;
	BSTG *right;

public:
	BSTG(HuffNode *data, int key)
	{
		this->data = data;
		this->key = key;
		this->left = nullptr;
		this->right = nullptr;
	}
	~BSTG() {}

	// Thêm một nút vào cây BST G
	void insert(HuffNode *data, int key)
	{
		if (key < this->key)
		{
			if (this->left == nullptr)
			{
				this->left = new BSTG(data, key);
			}
			else
			{
				this->left->insert(data, key);
			}
		}
		else if (key >= this->key)
		{
			if (this->right == nullptr)
			{
				this->right = new BSTG(data, key);
			}
			else
			{
				this->right->insert(data, key);
			}
		}
	}

	// In cây BST G theo trung thứ tự
	void printInorder()
	{
		if (this->left != nullptr)
		{
			this->left->printInorder();
		}
		cout << this->key << endl;
		if (this->right != nullptr)
		{
			this->right->printInorder();
		}
	}
};

// In cây BST G
void printBSTG(BSTG *root, string prefix = " ")
{
	if (root == nullptr)
	{
		return;
	}

	std::cout << prefix << root->key << "\n";

	// Duyệt và in các nút con
	printBSTG(root->left, prefix + "--");
	printBSTG(root->right, prefix + "--");
}

// In số lượng khách hàng trong G
void printGres()
{
	std::cout << "So luong khach hang trong G: \n";
	for (int i = 1; i <= MAXSIZE; i++)
	{
		std::cout << "-G" << i << ": " << Gcus[i].size() << endl;
		if (Gcus[i].size() == 0)
		{
			std::cout << "NULL";
		}
		for (size_t j = 0; j < Gcus[i].size(); j++)
		{
			std::cout << Gcus[i][j].second << " ";
		}
		cout << endl;
	}
}

// Cấu trúc dữ liệu cho MinHeap S
class MinHeap
{
public:
	vector<HeapNode *> heap;
	unordered_map<int, HeapNode *> labelMap;
	int nodeOrder;
	MinHeap() : nodeOrder(0) {}

	void reHeapUp(int idx)
	{
		int tempx = heap.size();
		if (idx != 0 && idx < tempx)
		{
			int parentIdx = (idx - 1) / 2;
			if ((heap[parentIdx]->num > heap[idx]->num) || ((heap[parentIdx]->num == heap[idx]->num && heap[parentIdx]->order2 > heap[idx]->order2)))
			{
				swap(heap[parentIdx], heap[idx]);
				reHeapUp(parentIdx);
			}
		}
	}

	void reHeapDown(int idx)
	{
		int leftChildIdx = 2 * idx + 1;
		int rightChildIdx = 2 * idx + 2;
		int smallest = idx;
		int tempx = heap.size();

		if (leftChildIdx < tempx && heap[leftChildIdx]->num <= heap[smallest]->num)
		{
			if ((heap[smallest]->order2 > heap[leftChildIdx]->order2 && heap[leftChildIdx]->num == heap[smallest]->num) || (heap[smallest]->num > heap[leftChildIdx]->num))
				smallest = leftChildIdx;
		}

		if (rightChildIdx < tempx && heap[rightChildIdx]->num <= heap[smallest]->num)
		{
			if ((heap[smallest]->order2 > heap[rightChildIdx]->order2 && heap[rightChildIdx]->num == heap[smallest]->num) || (heap[smallest]->num > heap[rightChildIdx]->num))
				smallest = rightChildIdx;
		}

		if (rightChildIdx < tempx && heap[rightChildIdx]->num == heap[leftChildIdx]->num && heap[rightChildIdx]->num < heap[smallest]->num)
		{
			if (heap[rightChildIdx]->order2 < heap[smallest]->order2)
				smallest = rightChildIdx;
			else
				smallest = leftChildIdx;
		}

		if (smallest != idx)
		{
			swap(heap[idx], heap[smallest]);
			reHeapDown(smallest);
		}
	}

	// Thêm một nút vào heap
	void insert(HeapNode *node)
	{
		this->nodeOrder++;
		int tempind = 0;
		if (labelMap.count(node->label))
		{
			labelMap[node->label]->order2 = this->nodeOrder;
			labelMap[node->label]->num++;
			for (size_t i = 0; i < heap.size(); i++)
			{
				if (heap[i]->label == node->label)
				{
					tempind = i;
					break;
				}
			}
			reHeapDown(tempind);
		}
		else
		{
			labelMap[node->label] = node;
			labelMap[node->label]->order2 = this->nodeOrder;
			labelMap[node->label]->order = this->nodeOrder;
			labelMap[node->label]->num++;
			heap.push_back(node);
			reHeapUp(heap.size() - 1);
		}
	}

	// Xóa một nút khỏi heap
	void remove(int label)
	{
		this->nodeOrder++;
		if (labelMap.count(label))
		{
			labelMap[label]->order2 = this->nodeOrder;
			int tempind = 0;
			for (size_t i = 0; i < heap.size(); i++)
			{
				if (heap[i]->label == label)
				{
					tempind = i;
					break;
				}
			}
			labelMap[label]->num--;
			if (labelMap[label]->num == 0)
			{
				swap(heap[tempind], heap[heap.size() - 1]);
				heap.pop_back();
				labelMap.erase(label);
			}
			reHeapDown(tempind);
			reHeapUp(tempind);
		}
	}

	// In heap
	void print(int idx = 0, string prefix = "")
	{
		int tempx = heap.size();
		if (idx >= tempx)
			return;
		std::cout << prefix << heap[idx]->label << ":" << heap[idx]->num << "\n";
		print(2 * idx + 1, prefix + "--");
		print(2 * idx + 2, prefix + "--");
	}
};

// Khởi tạo minheap
MinHeap *minheap = new MinHeap();

// In số lượng khách hàng trong S
void printSres()
{
	std::cout << "So luong khach hang trong S: \n";
	for (int i = 1; i <= MAXSIZE; i++)
	{
		std::cout << "-S" << i << ": " << Scus[i].size() << endl;
		if (Scus[i].size() == 0)
		{
			std::cout << "NULL";
		}
		for (size_t j = 0; j < Scus[i].size(); j++)
		{
			std::cout << Scus[i][j].second << " ";
		}
		cout << endl;
	}
}

// Thêm khách hàng vào nhà hàng S
void addCustomerS(HuffNode *root, int res)
{
	int temp = res % MAXSIZE + 1;
	Scus[temp].push_back({root, res});
	// std::cout << "Them vao khu S" << temp << endl;
	minheap->insert(SHeapNode[temp]);
	// for (int i = 1; i <= MAXSIZE; i++)
	// {
	// 	std::cout << "Cay BST G" << i << ":" << endl;
	// 	printBSTG(GcusTree[i]); // In cây BST G[i]
	// }
	// printGres(); // In số lượng khách hàng trong G
	// std::cout << "MinHeap S:" << endl;
	// minheap->print();
	// for (int i = 0; i < minheap->heap.size(); i++)
	// {
	// 	std::cout << minheap->heap[i]->label << ":" << minheap->heap[i]->num << ":" << minheap->heap[i]->order << ":" << minheap->heap[i]->order2 << " ";
	// }
	// cout << endl;
	// printSres(); // In số lượng khách hàng trong S
}

// Thêm khách hàng vào nhà hàng G
void addCustomerG(HuffNode *root, int res)
{
	int temp = res % MAXSIZE + 1;
	if (GcusTree[temp] == nullptr)
	{
		GcusTree[temp] = new BSTG(root, res);
	}
	else
	{
		GcusTree[temp]->insert(root, res);
	}
	Gcus[temp].push_back({root, res});
	// std::cout << "Them vao khu G" << temp << endl;
	// for (int i = 1; i <= MAXSIZE; i++)
	// {
	// 	std::cout << "Cay BST G" << i << ":" << endl;
	// 	printBSTG(GcusTree[i]); // In cây BST G[i]
	// }
	// printGres(); // In số lượng khách hàng trong G
	// std::cout << "MinHeap S:" << endl;
	// minheap->print();
	// for (int i = 0; i < minheap->heap.size(); i++)
	// {
	// 	std::cout << minheap->heap[i]->label << ":" << minheap->heap[i]->num << ":" << minheap->heap[i]->order << ":" << minheap->heap[i]->order2 << " ";
	// }
	// cout << endl;
	// printSres(); // In số lượng khách hàng trong S
}

void printRes()
{
	cout << "Khach trong nha hang: \n";
	for (size_t i = 0; i < customCaesar.size(); i++)
	{
		cout << customCaesar[i].second << " ";
	}
	cout << endl;
	printGres();
	printSres();
}

// LAPSE <NAME>
void LAPSE(string name)
{
	// std::cout << "---------------------\n"
	//   << "LAPSE "<< " - LINE: " << line << endl;
	if (!hasThreeDistinctChars(name))
	{
		// std::cout << "Khong du 3 chu. COOK!" << endl;
		return;
	}
	string s = MaHoaCaesar(name);
	vector<pair<char, int>> freq_vec;
	for (char c : s)
	{
		bool found = false;
		for (auto &p : freq_vec)
		{
			if (p.first == c)
			{
				p.second++;
				found = true;
				break;
			}
		}
		if (!found)
			freq_vec.push_back({c, 1});
	}

	s = MaHoaTen(name);

	HuffNode *root = buildHuff(freq_vec);
	// std::cout << "Cay Huffman: " << endl;
	// printTree(root);
	string binary = "";
	for (char c : s)
	{
		binary += findPath(root, c);
	}
	// std::cout << "Chuoi nhi phan: " << binary << endl;
	std::cout << "result = " << binaryToDecimal(binary) << endl;
	// std::cout << "root : ";
	// printTree(root);
	// std::cout << endl;
	customCaesar.push_back({root, binaryToDecimal(binary)});
	custom.push_back({root, binaryToDecimal(binary)});
	if (binaryToDecimal(binary) % 2 == 0)
	{
		addCustomerS(root, binaryToDecimal(binary));
	}
	else
	{
		addCustomerG(root, binaryToDecimal(binary));
	}
}

/*---------------------------------------------------*/
/*						KOKUSEN		   				 */
/*---------------------------------------------------*/

// Tìm node BSTG có date == HuffNode data
BSTG *findNodeBSTG(BSTG *root, HuffNode *data)
{
	if (root == nullptr)
	{
		return nullptr;
	}
	if (root->data == data)
	{
		return root;
	}
	BSTG *leftSearch = findNodeBSTG(root->left, data);
	if (leftSearch != nullptr) // Nếu tìm thấy trong cây con bên trái
	{
		return leftSearch;
	}
	// Nếu không tìm thấy trong cây con bên trái, tìm trong cây con bên phải
	return findNodeBSTG(root->right, data);
}

//  Xóa node trong cây BST G
BSTG *deleteNodeBSTG(BSTG *root, HuffNode *data)
{
	BSTG *delNode = findNodeBSTG(root, data);
	if (root == nullptr)
	{
		return root;
	}
	if (delNode->key < root->key)
	{
		root->left = deleteNodeBSTG(root->left, data);
	}
	else if (delNode->key > root->key)
	{
		root->right = deleteNodeBSTG(root->right, data);
	}
	else if (delNode->data != data)
	{
		root->right = deleteNodeBSTG(root->right, data);
	}
	else
	{
		if (root->left == nullptr)
		{
			BSTG *temp = root->right;
			delete root;
			return temp;
		}
		else if (root->right == nullptr)
		{
			BSTG *temp = root->left;
			delete root;
			return temp;
		}
		BSTG *temp = root->right;
		while (temp->left != nullptr)
		{
			temp = temp->left;
		}
		root->data = temp->data;
		root->key = temp->key;
		root->right = deleteNodeBSTG(root->right, temp->data);
	}
	return root;
}

// Lấy postorder của cây BST G
void getPostBSTG(BSTG *root, vector<int> &post)
{
	if (root != nullptr)
	{
		getPostBSTG(root->left, post);
		getPostBSTG(root->right, post);
		post.push_back(root->key);
	}
}

// Lấy preorder của cây BST G
void getPreBSTG(BSTG *root, vector<int> &pre)
{
	if (root != nullptr)
	{
		pre.push_back(root->key);
		getPreBSTG(root->left, pre);
		getPreBSTG(root->right, pre);
	}
}

int numOfWays(vector<int> &nums)
{
	int n = nums.size();
	vector<vector<int>> comb(n + 1, vector<int>(n + 1));
	comb[0][0] = 1;
	for (int i = 1; i <= n; ++i)
	{
		comb[i][0] = 1;
		for (int j = 1; j <= i; ++j)
		{
			comb[i][j] = (comb[i - 1][j - 1] + comb[i - 1][j]) % MAXSIZE;
		}
	}
	function<int(vector<int> &)> dfs = [&](vector<int> &nums)
	{
		int n = nums.size();
		if (n <= 2)
			return 1;
		vector<int> left, right;
		for (int i = 1; i < n; ++i)
		{
			if (nums[i] < nums[0])
				left.push_back(nums[i]);
			else
				right.push_back(nums[i]);
		}
		long long res = comb[n - 1][left.size()];
		res = res * dfs(left) % MAXSIZE;
		res = res * dfs(right) % MAXSIZE;
		return (int)res;
	};
	return dfs(nums) - 1;
}

// KOKUSEN
void KOKUSEN()
{
	// std::cout << "---------------------\n"
	// 		  << "KOKUSEN "
	// 		  << " - LINE: " << line << endl;

	for (int i = 1; i <= MAXSIZE; i++)
	{
		if (Gcus[i].size() == 0)
		{
			continue;
		}
		vector<int> pre;
		getPreBSTG(GcusTree[i], pre);
		int countBTSG = (numOfWays(pre) + 1) % MAXSIZE;
		// cout << countBTSG << endl;
		int tempz = Gcus[i].size();
		if (countBTSG > tempz)
			countBTSG = Gcus[i].size();
		for (int j = 0; j < countBTSG; j++)
		{
			GcusTree[i] = deleteNodeBSTG(GcusTree[i], Gcus[i][0].first);

			// Xóa khách hàng trong danh sách khách toàn nhà hàng
			for (size_t j = 0; j < customCaesar.size(); j++)
			{
				if (customCaesar[j].first == Gcus[i][0].first)
				{
					customCaesar.erase(customCaesar.begin() + j);
					break;
				}
			}

			// Xóa khách hàng trong danh sách khách hàng khu vực Gi
			// deleteHuffNode(Gcus[i][0].first);
			Gcus[i].erase(Gcus[i].begin());
		}
	}
}

/*---------------------------------------------------*/
/*						KEITEIKEN   				 */
/*---------------------------------------------------*/

// Hàm so sánh tùy chỉnh HeapNode
bool compare1(HeapNode *a, HeapNode *b)
{
	if (a->num == b->num)
	{
		return a->order2 < b->order2; // Nếu num bằng nhau, so sánh order2
	}
	return a->num < b->num; // Ngược lại, so sánh num
}

// KEITEIKEN <NUM>
void KEITEIKEN(int num)
{
	// std::cout << "---------------------\n"
	// 		  << "KEITEIKEN " << num << " - LINE: " << line << endl;
	// printRes();

	vector<HeapNode *> temp = SHeapNode;
	temp.erase(temp.begin());
	std::sort(temp.begin(), temp.end(), compare1);

	int count = 0;
	// minheap->print();
	// printRes();
	for (size_t i = 0; i < temp.size(); i++)
	{
		if (temp[i]->num == 0)
			continue;
		if (count == num)
			break;
		count++;
		int tempnum = (temp[i]->num > num) ? num : temp[i]->num;
		for (int j = 0; j < tempnum; j++)
		{
			// Giảm num trong node Heap
			minheap->remove(temp[i]->label);

			// In thông tin khách hàng đã cần xóa
			cout << Scus[temp[i]->label][0].second << "-" << temp[i]->label << endl;

			// Xóa khách hàng trong danh sách khách toàn nhà hàng
			for (size_t j = 0; j < customCaesar.size(); j++)
			{
				if (customCaesar[j].first == Scus[temp[i]->label][0].first)
				{
					customCaesar.erase(customCaesar.begin() + j);
					break;
				}
			}
			// Xóa khách hàng trong danh sách khách hàng khu vực S
			// deleteHuffNode(Scus[temp[i]->label][0].first);
			Scus[temp[i]->label].erase(Scus[temp[i]->label].begin());
		}
	}

	// // Tìm vị trí khu có số lượng khách hàng nhỏ nhất và chưa sử dụng lâu nhất trong S
	// int idxMin = 0;
	// HeapNode *min = SHeapNode[1];
	// // cout << "Khach hang can xoa: \n";
	// for (int i = 1; i <= MAXSIZE; i++)
	// {
	// 	if ((SHeapNode[i]->num == min->num && SHeapNode[i]->order2 < min->order2 && SHeapNode[i]->num != 0) || min->num == 0)
	// 	{
	// 		min = SHeapNode[i];
	// 		idxMin = i;
	// 	}
	// 	else if (SHeapNode[i]->num < min->num && SHeapNode[i]->num != 0)
	// 	{
	// 		min = SHeapNode[i];
	// 		idxMin = i;
	// 	}
	// }

	// // Nếu num > số khách hàng trong khu => num = số khách hàng trong khu
	// if (num >= min->num)
	// 	num = min->num;

	// for (int i = 0; i < num; i++)
	// {
	// 	// Giảm num trong node Heap
	// 	minheap->remove(min->label);

	// 	// In thông tin khách hàng đã cần xóa
	// 	cout << Scus[min->label][0].second << "-" << min->label << endl;

	// 	// Xóa khách hàng trong danh sách khách toàn nhà hàng
	// 	for (int j = 0; j < customCaesar.size(); j++)
	// 	{
	// 		if (customCaesar[j].first == Scus[min->label][0].first)
	// 		{
	// 			customCaesar.erase(customCaesar.begin() + j);
	// 			break;
	// 		}
	// 	}
	// 	// Xóa khách hàng trong danh sách khách hàng khu vực S
	// 	// deleteHuffNode(Scus[min->label][0].first);
	// 	Scus[min->label].erase(Scus[min->label].begin());
	// }

	// // In số lượng khách hàng trong S
	// // cout << "MinHeap S:" << endl;
	// // minheap->print();
	// // printSres();
}

/*---------------------------------------------------*/
/*						HAND    					 */
/*---------------------------------------------------*/

// In cây huffman theo trung thứ tự
void printInorder(HuffNode *root)
{
	if (root == nullptr)
		return;
	printInorder(root->left());
	if (root->it != '\0')
		cout << root->it << endl;
	else
		cout << root->wgt << endl;
	printInorder(root->right());
}

// HAND
void HAND()
{
	// std::cout << "---------------------\n"
	// 		  << "HAND "
	// 		  << " - LINE: " << line << endl;
	if (custom.size() != 0)
	{
		HuffNode *temp = custom.back().first;
		// std::cout << "root: ";
		// printTree(temp);
		// cout << endl;
		printInorder(temp);
	}
}

/*---------------------------------------------------*/
/*						LIMITLESS					 */
/*---------------------------------------------------*/

// LIMITLESS <NUM>
void LIMITLESS(int num)
{
	// cout << "---------------------\n"
	// 	 << "LIMITLESS " << num << " - LINE: " << line << endl;
	// printRes();
	// for (int i = 0; i < customCaesar.size(); i++)
	// {
	// 	if (customCaesar[i].second == num)
	// 	{
	// 		cout << "-" << customCaesar[i].second;
	// 	}
	// }
	// cout << endl;

	if (num > MAXSIZE || num < 1)
	{
		// cout << "Khong ton tai khu vuc " << num << endl;
		return;
	}

	if (GcusTree[num] == nullptr)
	{
		// cout << "Khong co khach hang trong khu vuc " << num << endl;
		return;
	}

	// In cây BST G khu vực NUM theo trung thứ tự
	// cout << "Cay BST G" << num << ":" << endl;
	GcusTree[num]->printInorder();
}

/*---------------------------------------------------*/
/*						CLEAVE  					 */
/*---------------------------------------------------*/
// In min heap theo tiền thứ tự
void printPreMinheap(vector<HeapNode *> heap, int num, int i = 0)
{
	int tempx = heap.size();
	if (i < tempx)
	{
		// In nút hiện tại
		int temp = (heap[i]->num > num) ? num : heap[i]->num;
		for (int j = 0; j < temp; j++)
		{
			int len = Scus[heap[i]->label].size();
			cout << heap[i]->label << "-" << Scus[heap[i]->label][len - 1 - j].second << endl;
		}
		// Duyệt qua nút con bên trái
		printPreMinheap(heap, num, 2 * i + 1);

		// Duyệt qua nút con bên phải
		printPreMinheap(heap, num, 2 * i + 2);
	}
}

// CLEAVE <NUM>
void CLEAVE(int num)
{
	printPreMinheap(minheap->heap, num, 0);
	// cout << endl;
	// printRes();
}
/*---------------------------------------------------*/
/*---------------------------------------------------*/

// Hàm hủy cây Huffman
void deleteHuffNode(HuffNode *root)
{
	if (root == nullptr)
		return;
	deleteHuffNode(root->left());
	deleteHuffNode(root->right());
	delete root;
}

// Hàm hủy cây BST G
void deleteBSTG(BSTG *root)
{
	if (root == nullptr)
		return;
	deleteBSTG(root->left);
	deleteBSTG(root->right);
	delete root;
}

// Hàm hủy minheap
void deleteMinheap(MinHeap *minheap)
{
	delete minheap;
}

// Hàm hủy HeapNode
void deleteHeapNode(HeapNode *heapnode)
{
	delete heapnode;
}

// Hàm hủy tất cả các cấu trúc dữ liệu
void deleteAll()
{
	// Hủy cây Huffman
	for (size_t i = 0; i < custom.size(); i++)
	{
		deleteHuffNode(custom[i].first);
	}
	// Hủy cây BST G
	for (int i = 1; i <= MAXSIZE; i++)
	{
		deleteBSTG(GcusTree[i]);
	}
	// Hủy minheap
	deleteMinheap(minheap);
	// Hủy HeapNode
	for (int i = 1; i <= MAXSIZE; i++)
	{
		deleteHeapNode(SHeapNode[i]);
	}
}
