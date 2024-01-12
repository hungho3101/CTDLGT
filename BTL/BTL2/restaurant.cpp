#include "main.h"
int MAXSIZE;
int line = 0;
int numOrder = 0;
class HuffNode;
class BSTG;
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
		if (str == "MAXSIZE") // MAXSIZE <NUM>
		{
			line--;
			ss >> maxsize;
			MAXSIZE = stoi(maxsize);
			GcusTree.resize(MAXSIZE + 1, nullptr);
			Gcus.resize(MAXSIZE + 1);
			Scus.resize(MAXSIZE + 1);
			SHeapNode.resize(MAXSIZE + 1);
			for (int i = 1; i <= MAXSIZE; i++)
				SHeapNode[i] = new HeapNode(i, 0);
		}
		else if (str == "LAPSE") // LAPSE <NAME>
		{
			ss >> name;
			LAPSE(name);
		}
		else if (str == "KOKUSEN")
			KOKUSEN();
		else if (str == "KEITEIKEN") // KEITEIKEN <NUM>
		{
			ss >> num;
			KEITEIKEN(stoi(num));
		}
		else if (str == "HAND") // HAND
			HAND();
		else if (str == "LIMITLESS") // LIMITLESS <NUM>
		{
			ss >> num;
			LIMITLESS(stoi(num));
		}
		else if (str == "CLEAVE") // CLEAVE <NUM>
		{
			ss >> num;
			CLEAVE(stoi(num));
		}
	}
	deleteAll();
}
bool hasThreeDistinctChars(const string &str)
{
	set<char> distinct_chars;
	for (char c : str)
	{
		distinct_chars.insert(c);
		if (distinct_chars.size() >= 3)
			return true;
	}
	return false;
}
string MaHoaCaesar(string s)
{
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
		sorted_str += string(p.second, p.first);
	s = sorted_str;
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
	return s;
}
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
		wgt = l->wgt + r->wgt; // Tính trọng số
		lc = l;
		rc = r;
		order = numOrder++;
	}
};
int getHeight(HuffNode *node)
{
	if (node == nullptr)
		return 0;
	int leftHeight = getHeight(node->lc);
	int rightHeight = getHeight(node->rc);
	return std::max(leftHeight, rightHeight) + 1;
}
HuffNode *rightRotate(HuffNode *y)
{
	HuffNode *x = y->lc;
	HuffNode *T2 = x->rc;
	x->rc = y;
	y->lc = T2;
	return x;
}
HuffNode *leftRotate(HuffNode *x)
{
	HuffNode *y = x->rc;
	HuffNode *T2 = y->lc;
	y->lc = x;
	x->rc = T2;
	return y;
}
int getBalance(HuffNode *N)
{
	if (N == nullptr)
		return 0;
	return getHeight(N->lc) - getHeight(N->rc);
}
HuffNode *balanceHuffTree(HuffNode *root)
{
	if (root == nullptr || getHeight(root) < 3)
		return root;
	int balance = getBalance(root);
	if (balance > 1 && getBalance(root->lc) >= 0)
		root = rightRotate(root);
	else if (balance < -1 && getBalance(root->rc) <= 0)
		root = leftRotate(root);
	else if (balance > 1 && getBalance(root->lc) < 0)
	{
		root->lc = leftRotate(root->lc);
		root = rightRotate(root);
	}
	else if (balance < -1 && getBalance(root->rc) > 0)
	{
		root->rc = rightRotate(root->rc);
		root = leftRotate(root);
	}
	return root;
}
HuffNode *balpre(HuffNode *root, int &count)
{
	if (root == nullptr || count == 1)
		return root;
	if (getBalance(root) > 1 || getBalance(root) < -1)
	{
		root = balanceHuffTree(root);
		count++;
	}

	root->lc = balpre(root->lc, count);
	root->rc = balpre(root->rc, count);
	return root;
}
struct compare
{
	bool operator()(HuffNode *l, HuffNode *r)
	{
		if (l->wgt == r->wgt)
		{
			return l->order > r->order; // Use the order of insertion as the second comparison criterion
		}
		return l->wgt > r->wgt;
	}
};
HuffNode *buildHuff(vector<pair<char, int>> &freqs)
{
	priority_queue<HuffNode *, vector<HuffNode *>, compare> forest;
	for (const auto &freq : freqs)
		forest.push(new HuffNode(freq.first, freq.second));
	while (forest.size() > 1)
	{
		HuffNode *temp1 = forest.top();
		forest.pop();
		HuffNode *temp2 = forest.top();
		forest.pop();
		HuffNode *temp = new HuffNode(temp1, temp2);
		for (int i = 0; i < 3; i++)
		{
			int count = 0;
			temp = balpre(temp, count);
		}
		temp->order = numOrder++;
		forest.push(temp);
	}
	return forest.top();
}
string findPath(HuffNode *root, char value, string path = "")
{
	if (root == nullptr)
		return "";
	if (root->it == value)
		return path;
	string leftPath = findPath(root->lc, value, path + "0");
	string rightPath = findPath(root->rc, value, path + "1");
	if (!leftPath.empty())
		return leftPath;
	if (!rightPath.empty())
		return rightPath;
	return "";
}
int binaryToDecimal(string binary)
{
	int decimal = 0;
	int base = 1;
	int len = binary.length();
	int start = max(0, len - 10);
	for (int i = start; i < len; i++)
	{
		if (binary[i] == '1')
			decimal += base;
		base = base * 2;
	}
	return decimal;
}
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
	void insert(HuffNode *data, int key)
	{
		if (key < this->key)
		{
			if (this->left == nullptr)
				this->left = new BSTG(data, key);
			else
				this->left->insert(data, key);
		}
		else if (key >= this->key)
		{
			if (this->right == nullptr)
				this->right = new BSTG(data, key);
			else
				this->right->insert(data, key);
		}
	}
	void printInorder()
	{
		if (this->left != nullptr)
			this->left->printInorder();
		cout << this->key << "\n";
		if (this->right != nullptr)
			this->right->printInorder();
	}
};
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
};
MinHeap *minheap = new MinHeap();
void addCustomerS(HuffNode *root, int res)
{
	int temp = res % MAXSIZE + 1;
	Scus[temp].push_back({root, res});
	minheap->insert(SHeapNode[temp]);
}
void addCustomerG(HuffNode *root, int res)
{
	int temp = res % MAXSIZE + 1;
	if (GcusTree[temp] == nullptr)
		GcusTree[temp] = new BSTG(root, res);
	else
		GcusTree[temp]->insert(root, res);
	Gcus[temp].push_back({root, res});
}
void LAPSE(string name)
{
	if (!hasThreeDistinctChars(name))
		return;
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
	string binary = "";
	for (char c : s)
		binary += findPath(root, c);
	customCaesar.push_back({root, binaryToDecimal(binary)});
	custom.push_back({root, binaryToDecimal(binary)});
	if (binaryToDecimal(binary) % 2 == 0)
		addCustomerS(root, binaryToDecimal(binary));
	else
		addCustomerG(root, binaryToDecimal(binary));
}
BSTG *findNodeBSTG(BSTG *root, HuffNode *data)
{
	if (root == nullptr)
		return nullptr;
	if (root->data == data)
		return root;
	BSTG *leftSearch = findNodeBSTG(root->left, data);
	if (leftSearch != nullptr) // Nếu tìm thấy trong cây con bên trái
		return leftSearch;
	return findNodeBSTG(root->right, data);
}
BSTG *deleteNodeBSTG(BSTG *root, HuffNode *data)
{
	BSTG *delNode = findNodeBSTG(root, data);
	if (root == nullptr)
		return root;
	if (delNode->key < root->key)
		root->left = deleteNodeBSTG(root->left, data);
	else if (delNode->key > root->key)
		root->right = deleteNodeBSTG(root->right, data);
	else if (delNode->data != data)
		root->right = deleteNodeBSTG(root->right, data);
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
void KOKUSEN()
{
	for (int i = 1; i <= MAXSIZE; i++)
	{
		if (Gcus[i].size() == 0)
			continue;
		vector<int> pre;
		getPreBSTG(GcusTree[i], pre);
		int countBTSG = (numOfWays(pre) + 1) % MAXSIZE;
		int tempz = Gcus[i].size();
		if (countBTSG > tempz)
			countBTSG = Gcus[i].size();
		for (int j = 0; j < countBTSG; j++)
		{
			GcusTree[i] = deleteNodeBSTG(GcusTree[i], Gcus[i][0].first);
			for (size_t j = 0; j < customCaesar.size(); j++)
			{
				if (customCaesar[j].first == Gcus[i][0].first)
				{
					customCaesar.erase(customCaesar.begin() + j);
					break;
				}
			}
			Gcus[i].erase(Gcus[i].begin());
		}
	}
}
bool compare1(HeapNode *a, HeapNode *b)
{
	if (a->num == b->num)
		return a->order2 < b->order2; // Nếu num bằng nhau, so sánh order2
	return a->num < b->num; // Ngược lại, so sánh num
}
void KEITEIKEN(int num)
{
	vector<HeapNode *> temp = SHeapNode;
	temp.erase(temp.begin());
	std::sort(temp.begin(), temp.end(), compare1);
	int count = 0;
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
			minheap->remove(temp[i]->label);
			cout << Scus[temp[i]->label][0].second << "-" << temp[i]->label << "\n";
			for (size_t j = 0; j < customCaesar.size(); j++)
			{
				if (customCaesar[j].first == Scus[temp[i]->label][0].first)
				{
					customCaesar.erase(customCaesar.begin() + j);
					break;
				}
			}
			Scus[temp[i]->label].erase(Scus[temp[i]->label].begin());
		}
	}
}
void printInorder(HuffNode *root)
{
	if (root == nullptr)
		return;
	printInorder(root->lc);
	if (root->it != '\0')
		cout << root->it << "\n";
	else
		cout << root->wgt << "\n";
	printInorder(root->rc);
}
void HAND()
{
	if (custom.size() != 0)
	{
		HuffNode *temp = custom.back().first;
		printInorder(temp);
	}
}
void LIMITLESS(int num)
{
	if (num > MAXSIZE || num < 1)
		return;
	if (GcusTree[num] == nullptr)
		return;
	GcusTree[num]->printInorder();
}
void printPreMinheap(vector<HeapNode *> heap, int num, int i = 0)
{
	int tempx = heap.size();
	if (i < tempx)
	{
		int temp = (heap[i]->num > num) ? num : heap[i]->num;
		for (int j = 0; j < temp; j++)
		{
			int len = Scus[heap[i]->label].size();
			cout << heap[i]->label << "-" << Scus[heap[i]->label][len - 1 - j].second << "\n";
		}
		printPreMinheap(heap, num, 2 * i + 1);
		printPreMinheap(heap, num, 2 * i + 2);
	}
}
void CLEAVE(int num)
{
	printPreMinheap(minheap->heap, num, 0);
}
void deleteHuffNode(HuffNode *root)
{
	if (root == nullptr)
		return;
	deleteHuffNode(root->lc);
	deleteHuffNode(root->rc);
	delete root;
}
void deleteBSTG(BSTG *root)
{
	if (root == nullptr)
		return;
	deleteBSTG(root->left);
	deleteBSTG(root->right);
	delete root;
}
void deleteMinheap(MinHeap *minheap)
{
	delete minheap;
}
void deleteHeapNode(HeapNode *heapnode)
{
	delete heapnode;
}
void deleteAll()
{
	for (size_t i = 0; i < custom.size(); i++)
	{
		deleteHuffNode(custom[i].first);
	}
	for (int i = 1; i <= MAXSIZE; i++)
	{
		deleteBSTG(GcusTree[i]);
	}
	deleteMinheap(minheap);
	for (int i = 1; i <= MAXSIZE; i++)
	{
		deleteHeapNode(SHeapNode[i]);
	}
}
