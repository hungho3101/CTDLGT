#include "main.h"

class imp_res : public Restaurant
{
public:
    int numDiningCustomers = 0;                // số khách trong bàn ăn
    int numWaitingCustomers = 0;               // số khách trong hàng đợi
    customer *X = nullptr;                     // vị trí có sự thay đổi mới nhất
    customer *firstWaitingCustomer = nullptr;  // vị trí đầu tiên hàng đợi
    customer *longestDiningCustomer = nullptr; // người có thời gian ăn lâu nhất

    imp_res(){};
    ~imp_res()
    {
        for (int i = 0; i < numDiningCustomers; ++i)
        {
            if (this->X == this->X->next)
                delete this->X;
            else
            {
                customer *temp = this->X;
                this->X = this->X->next;
                delete temp;
            }
        }
        while (this->longestDiningCustomer != nullptr)
        {
            customer *temp = this->longestDiningCustomer;
            this->longestDiningCustomer = this->longestDiningCustomer->next;
            delete temp;
        }
    }

    // Thứ tự vào ăn trước sau
    void CustomerOrder(customer *cus)
    {
        customer *newCustomerNode = new customer(cus->name, cus->energy, nullptr, nullptr);
        if (longestDiningCustomer == nullptr)
        {
            longestDiningCustomer = newCustomerNode;
            return;
        }
        customer *currentNode = longestDiningCustomer;
        while (currentNode->next != nullptr)
        {
            currentNode = currentNode->next;
        }
        currentNode->next = newCustomerNode;
        newCustomerNode->prev = currentNode;
    }

    // Thêm khách vào hàng chờ
    void addWaitingCus(customer *cus)
    {
        (this->numWaitingCustomers)++;
        if (this->firstWaitingCustomer == nullptr)
        {
            this->firstWaitingCustomer = cus;
            this->firstWaitingCustomer->next = nullptr;
        }
        else
        {
            customer *current = this->firstWaitingCustomer;
            while (current->next != nullptr)
            {
                current = current->next;
            }
            current->next = cus;
            cus->next = nullptr;
        }
    }

    // Thêm khách vào bàn ăn khi số khách trong bàn < MAXSIZE/2
    void addDeskCus_1(customer *cus)
    {
        (this->numDiningCustomers)++;
        // Chưa có khách nào ăn
        if (!X)
        {
            this->X = cus;
            this->X->next = this->X;
            this->X->prev = this->X;
        }

        // Xếp khách theo chiều kim đồng hồ
        else if (cus->energy >= this->X->energy)
        {
            cus->next = this->X->next;
            cus->prev = this->X;
            this->X->next->prev = cus;
            this->X->next = cus;
            this->X = cus;
        }

        // Xếp khách ngược chiều kim đồng hồ
        else
        {
            cus->next = this->X;
            cus->prev = this->X->prev;
            this->X->prev->next = cus;
            this->X->prev = cus;
            this->X = cus;
        }
    }

    // Thêm khách vào bàn ăn khi số khách trong bàn >= MAXSIZE/2
    void addDeskCus_2(customer *cus)
    {
        // Tìm hiệu trị tuyệt đối lớn nhất RES
        int res = INT16_MIN;
        customer *temp = this->X;
        do
        {
            if (abs(cus->energy - temp->energy) > res)
            {
                res = abs(cus->energy - temp->energy);
            }
            temp = temp->next;
        } while (temp != this->X);
        //  Tìm vị trí RES đầu tiên và thêm khách vào
        do
        {
            if (abs(cus->energy - temp->energy) == res)
            {
                // Xếp khách theo chiều kim đồng hồ
                if (cus->energy - temp->energy >= 0)
                {
                    cus->next = temp->next;
                    cus->prev = temp;
                    temp->next->prev = cus;
                    temp->next = cus;
                    this->X = cus;
                }

                // Xếp khách ngược chiều kim đồng hồ
                else if (cus->energy - temp->energy < 0)
                {
                    cus->next = temp;
                    cus->prev = temp->prev;
                    temp->prev->next = cus;
                    temp->prev = cus;
                    this->X = cus;
                }
                (this->numDiningCustomers)++;
                break;
            }
            temp = temp->next;
        } while (temp != this->X);
    }

    // Kiểm tra tên độc nhất
    bool isOnlyName(customer *cus)
    {
        customer *temp = this->firstWaitingCustomer;
        while (temp != nullptr)
        {
            if (temp->name == cus->name)
                return false;
            temp = temp->next;
        }
        if (this->numDiningCustomers > 0)
        {
            temp = this->X;
            do
            {
                if (temp->name == cus->name)
                    return false;
                temp = temp->next;
            } while (temp != this->X);
        }
        return true;
    }

    // Kiểm tra hàng chờ đầy chưa
    bool isWaitFull(int numWaitingCustomers)
    {
        return numWaitingCustomers >= MAXSIZE;
    }

    // Kiểm tra bàn ăn đầy chưa
    bool isDeskFull(int numDiningCustomers)
    {
        return numDiningCustomers >= MAXSIZE;
    }

    void RED(string name, int energy)
    {
        customer *cus = new customer(name, energy, nullptr, nullptr);
        if (cus->energy == 0)
        {
            return;
        }
        if (!isOnlyName(cus))
        {
            return;
        }
        if (isDeskFull(numDiningCustomers))
        {
            if (isWaitFull(numWaitingCustomers))
            {
                return;
            }
            else
            {
                addWaitingCus(cus);
                CustomerOrder(cus);
                return;
            }
        }
        else if ((this->numDiningCustomers < MAXSIZE / 2) || (MAXSIZE == 1 && this->numDiningCustomers == 0))
        {
            addDeskCus_1(cus);
            CustomerOrder(cus);
        }
        else
        {
            addDeskCus_2(cus);
            CustomerOrder(cus);
        }
    }

    // Đuổi khách tên Name khỏi bàn ăn
    void evictDeskName(string name)
    {
        // Xóa khách trong bàn
        // Tìm khách hàng cần đuổi
        customer *tempX = this->X;
        while (tempX->name != name)
            tempX = tempX->next;
        // Cập nhật các con trỏ để loại bỏ khách hàng khỏi danh sách
        tempX->next->prev = tempX->prev;
        tempX->prev->next = tempX->next;
        if (tempX->energy > 0)
            this->X = tempX->next;
        else
            this->X = tempX->prev;
        if (numDiningCustomers == 1)
            this->X = nullptr;

        // Giảm số lượng khách hàng
        this->numDiningCustomers--;

        // Loại bỏ khách hàng khỏi danh sách tên
        customer *tempName2;
        customer *tempName = this->longestDiningCustomer;

        if (tempName->name == name)
        {
            tempName2 = this->longestDiningCustomer;
            this->longestDiningCustomer = this->longestDiningCustomer->next;
            if (this->longestDiningCustomer != nullptr)
                this->longestDiningCustomer->prev = nullptr;
        }
        else
        {
            while (1)
            {
                if (tempName->next->name == name)
                    break;
                tempName = tempName->next;
            }
            tempName2 = tempName->next;
            if (tempName2->next != nullptr)
            {
                tempName2->next->prev = tempName2->prev;
                tempName->next = tempName->next->next;
            }
            else
            {
                tempName2->prev->next = nullptr;
            }
        }
        // Giải phóng bộ nhớ liên quan đến đối tượng khách hàng
        delete tempX;     // Giải phóng Customer
        delete tempName2; // Giải phóng customer
    }

    // Đuổi khách tên Name khỏi hàng đợi
    void evictWaitName(string name)
    {
        // Xóa khách trong hàng
        // Tìm khách hàng cần đuổi
        customer *tempX = this->firstWaitingCustomer;
        while (1)
        {
            if (tempX->name == name || tempX->next->name == name)
                break;
            tempX = tempX->next;
        }
        // Cập nhật các con trỏ để loại bỏ khách hàng khỏi danh sách
        if (name == this->firstWaitingCustomer->name)
        {
            tempX = this->firstWaitingCustomer;
            this->firstWaitingCustomer = this->firstWaitingCustomer->next;
        }
        else
        {
            customer *temp = tempX->next;
            tempX->next = tempX->next->next;
            tempX = temp;
        }
        // Giảm số lượng khách hàng
        this->numWaitingCustomers--;

        // Loại bỏ khách hàng khỏi danh sách tên
        customer *tempName2;
        customer *tempName = this->longestDiningCustomer;
        if (tempName->name == name)
        {
            tempName2 = this->longestDiningCustomer;
            this->longestDiningCustomer = this->longestDiningCustomer->next;
            this->longestDiningCustomer->prev = nullptr;
        }
        else
        {
            while (1)
            {
                if (tempName->next->name == name)
                    break;
                tempName = tempName->next;
            }
            tempName2 = tempName->next;
            if (tempName2->next != nullptr)
            {
                tempName2->next->prev = tempName2->prev;
                tempName->next = tempName->next->next;
            }
            else
                tempName->next = nullptr;
        }
        // Giải phóng bộ nhớ liên quan đến đối tượng khách hàng
        delete tempX;     // Giải phóng Customer
        delete tempName2; // Giải phóng customer
    }

    bool IsWait(string name)
    {
        customer *tempCus = this->firstWaitingCustomer;
        bool boolWait = false;
        while (tempCus != nullptr)
        {
            if (name == tempCus->name)
                boolWait = true;
            tempCus = tempCus->next;
        }
        return boolWait;
    }
    void BLUE(int num)
    {
        if (num > MAXSIZE)
            num = MAXSIZE;
        if (num > numDiningCustomers)
            num = numDiningCustomers;
        if (num == numDiningCustomers)
        {
            for (int i = 0; i < num; ++i)
                evictDeskName(this->X->name);
        }
        else
        {
            customer *tempCus = this->longestDiningCustomer;
            int dem = 0;
            while (tempCus != nullptr)
            {
                if (dem == num)
                    break;
                if (!IsWait(tempCus->name))
                {
                    evictDeskName(tempCus->name);
                    dem++;
                }
                tempCus = tempCus->next;
            }
        }
        while (this->firstWaitingCustomer != nullptr && numDiningCustomers < MAXSIZE)
        {
            customer *tempcus = this->firstWaitingCustomer;
            this->firstWaitingCustomer = this->firstWaitingCustomer->next;
            this->numWaitingCustomers--;
            if ((this->numDiningCustomers < MAXSIZE / 2) || (MAXSIZE == 1 && this->numDiningCustomers == 0))
            {
                addDeskCus_1(tempcus);
            }
            else
            {
                addDeskCus_2(tempcus);
            }
        }

    }

    // Trả về khách hàng thứ n trong hàng chờ
    customer *WaitIndex(int n)
    {
        n++;
        customer *tempWait = this->firstWaitingCustomer;
        if (n == 1)
            tempWait = this->firstWaitingCustomer;
        else
            for (int i = 1; i < n; ++i)
            {
                tempWait = tempWait->next;
            }
        return tempWait;
    }

    // Hàm lấy index thứ tự khách tên Name
    int OrderIndex(string name)
    {
        customer *tempCus = this->longestDiningCustomer;
        int index = 0;
        while (tempCus->name != name)
        {
            index++;
            tempCus = tempCus->next;
        }
        return index;
    }

    void insertsort(int index, int num, int incr, int &step)
    {
        for (int i = incr; i < num; i += incr)
            for (int j = i; j >= incr && abs(WaitIndex(j + index)->energy) >= abs(WaitIndex(j - incr + index)->energy); j -= incr)
            {
                if (abs(WaitIndex(j + index)->energy) > abs(WaitIndex(j - incr + index)->energy))
                {
                    int tempEnergy = WaitIndex(j + index)->energy;
                    string tempName = WaitIndex(j + index)->name;
                    WaitIndex(j + index)->energy = WaitIndex(j - incr + index)->energy;
                    WaitIndex(j + index)->name = WaitIndex(j - incr + index)->name;
                    WaitIndex(j - incr + index)->energy = tempEnergy;
                    WaitIndex(j - incr + index)->name = tempName;
                    step++;
                }
                else if (OrderIndex(WaitIndex(j + index)->name) < OrderIndex(WaitIndex(j - incr + index)->name))
                {
                    int tempEnergy = WaitIndex(j + index)->energy;
                    string tempName = WaitIndex(j + index)->name;
                    WaitIndex(j + index)->energy = WaitIndex(j - incr + index)->energy;
                    WaitIndex(j + index)->name = WaitIndex(j - incr + index)->name;
                    WaitIndex(j - incr + index)->energy = tempEnergy;
                    WaitIndex(j - incr + index)->name = tempName;
                    step++;
                }
            }
    }

    // Hàm sắp xếp Shell sort xếp hàng chờ
    int shellSort()
    {
        if (this->numWaitingCustomers == 0 || this->numWaitingCustomers == 1)
            return 0;
        // Tìm vị trí lớn nhất sau cùng
        string temp = this->firstWaitingCustomer->name;
        customer *tempCus = this->firstWaitingCustomer;
        customer *LastMax = nullptr;
        int max = -1;
        while (tempCus != nullptr)
        {
            if (max <= abs(tempCus->energy))
            {
                if (max == abs(tempCus->energy))
                {
                    if (OrderIndex(LastMax->name) < OrderIndex(tempCus->name))
                        LastMax = tempCus;
                }
                else
                {
                    max = abs(tempCus->energy);
                    LastMax = tempCus;
                }
            }
            tempCus = tempCus->next;
        }

        // Đếm số phần tử
        tempCus = this->firstWaitingCustomer;
        int n = 0;
        while (tempCus != LastMax)
        {
            ++n;
            if (tempCus->next == LastMax)
                ++n;
            tempCus = tempCus->next;
        }
        // Sắp xếp
        int step = 0;
        for (int i = n / 2; i > 2; i /= 2)
            for (int j = 0; j < i; j++)
                insertsort(j, n - j, i, step);
        insertsort(0, n, 1, step);
        return step;
    }

    void PURPLE()
    {
        int xxx = shellSort();
        BLUE(xxx % MAXSIZE);
    }

    // Đảo ngược vị trí khách theo ngược chiều kim đồng hồ từ vị trí X
    void REVERSAL()
    {
        if (this->X == nullptr)
            return;
        string tempName = this->X->name;
        customer *temphead = this->X;
        customer *temptail = this->X->next;

        // Đảo chỗ chú thuật sư
        while (temphead != temptail)
        {
            while (temphead->energy < 0)
            {
                if (temphead == temptail)
                    break;
                temphead = temphead->prev;
            }
            while (temptail->energy < 0)
            {
                if (temptail == temphead)
                    break;
                temptail = temptail->next;
            }
            if (temphead == temptail)
                break;
            string tempName2 = temphead->name;
            int tempEnergy = temphead->energy;
            temphead->name = temptail->name;
            temphead->energy = temptail->energy;
            temptail->name = tempName2;
            temptail->energy = tempEnergy;
            if (temphead->prev == temptail)
                break;
            temphead = temphead->prev;
            temptail = temptail->next;
        }

        temphead = this->X;
        temptail = this->X->next;

        // Đảo chỗ oán linh
        while (temphead != temptail)
        {
            while (temphead->energy > 0)
            {
                if (temphead == temptail)
                    break;
                temphead = temphead->prev;
            }
            while (temptail->energy > 0)
            {
                if (temptail == temphead)
                    break;
                temptail = temptail->next;
            }
            if (temphead == temptail)
                break;
            string tempName2 = temphead->name;
            int tempEnergy = temphead->energy;
            temphead->name = temptail->name;
            temphead->energy = temptail->energy;
            temptail->name = tempName2;
            temptail->energy = tempEnergy;
            if (temphead->prev == temptail)
                break;
            temphead = temphead->prev;
            temptail = temptail->next;
        }

        // Đặt lại X
        while (temphead->name != tempName)
            temphead = temphead->next;
        this->X = temphead;
    }

    // Trả về khách hàng thứ n trong sau X
    customer *DeskIndex(int n)
    {
        n++;
        customer *tempDesk = this->X;
        if (n == 1)
            tempDesk = this->X;
        else
            for (int i = 1; i < n; ++i)
            {
                tempDesk = tempDesk->next;
            }
        return tempDesk;
    }

    void UNLIMITED_VOID()
    {
        int min_sum = INT8_MAX;
        int LENGHT = 0;
        customer *HEADSUB = nullptr;

        // Dãy con vòng lại X; nêu không cho vào lại X thì xóa vòng lặp for ngoải cùng -> đưa xóa thisX.next
        for (int l = 0; l < this->numDiningCustomers; l++)
        {
            // Khởi tạo biến lưu trữ dãy con có tổng giá trị nhỏ nhất và độ dài của dãy con đó
            customer *headsub = nullptr;
            int lenght = 0;

            // Duyệt qua các dãy con có độ dài từ 4 trở lên, bắt đầu từ phần tử thứ i
            for (int j = 0; j < this->numDiningCustomers; j++)
            {
                // Tính tổng của dãy con
                int sum = 0;
                for (int k = 0; k <= j; k++)
                {
                    sum += DeskIndex(k)->energy;
                }

                // Nếu tổng của dãy con nhỏ hơn hoặc bằng tổng của dãy con đã lưu trữ trước đó và độ dài của dãy con lớn hơn hoặc bằng 4
                if (sum <= min_sum && (j + 1) > 3)
                {
                    lenght = j + 1;
                    if (sum < min_sum)
                    {
                        min_sum = sum;
                        headsub = DeskIndex(0);
                        HEADSUB = headsub;
                        LENGHT = lenght;
                    }
                    else if (LENGHT <= lenght)
                    {
                        min_sum = sum;
                        headsub = DeskIndex(0);
                        HEADSUB = headsub;
                        LENGHT = lenght;
                    }
                }
            }
            this->X = this->X->next;
        }

        if (HEADSUB != nullptr)
        {
            int min = INT32_MIN;
            customer *lastsub = HEADSUB;
            customer *minsub = HEADSUB;

            for (int i = 0; i < LENGHT; i++)
            {
                if (minsub->energy > lastsub->energy)
                    minsub = lastsub;
                lastsub = lastsub->next;
            }
            // In ra dãy con có tổng giá trị nhỏ nhất
            for (int i = 0; i < LENGHT; i++)
            {
                minsub->print();
                minsub = minsub->next;
                if (minsub == lastsub)
                    minsub = HEADSUB;
            }
        }
    }

    void domainexpansion()
    {
        if (this->numDiningCustomers == 0)
            return;
        int energy_neg = 0;
        int energy_pos = 0;
        customer *temp = this->longestDiningCustomer;

        while (temp != nullptr)
        {
            if (temp->energy > 0)
                energy_pos += temp->energy;
            else
                energy_neg += temp->energy;
            temp = temp->next;
        }
        customer *tempNL = this->longestDiningCustomer;
        while (tempNL->next != nullptr)
            tempNL = tempNL->next;
        if (energy_pos >= abs(energy_neg))
        {
            while (tempNL != nullptr)
            {
                if (tempNL->energy < 0)
                {
                    tempNL->print();
                }
                if (tempNL != nullptr)
                    tempNL = tempNL->prev;
            }

            tempNL = this->longestDiningCustomer;
            while (tempNL != nullptr)
            {
                if (tempNL->energy < 0)
                {
                    customer *tempCus = this->firstWaitingCustomer;
                    bool isWait = false;
                    while (tempCus != nullptr)
                    {
                        if (tempCus->name == tempNL->name)
                            isWait = true;
                        tempCus = tempCus->next;
                    }
                    customer *tempNL2 = tempNL;
                    tempNL = tempNL->next;
                    if (isWait)
                        evictWaitName(tempNL2->name);
                    else
                        evictDeskName(tempNL2->name);
                    continue;
                }
                if (tempNL != nullptr)
                    tempNL = tempNL->next;
            }
        }
        else
        {
            while (tempNL != nullptr)
            {
                if (tempNL->energy > 0)
                {
                    tempNL->print();
                }
                if (tempNL != nullptr)
                    tempNL = tempNL->prev;
            }

            tempNL = this->longestDiningCustomer;
            while (tempNL != nullptr)
            {
                if (tempNL->energy > 0)
                {
                    customer *tempCus = this->firstWaitingCustomer;
                    bool isWait = false;
                    while (tempCus != nullptr)
                    {
                        if (tempCus->name == tempNL->name)
                            isWait = true;
                        tempCus = tempCus->next;
                    }
                    customer *tempNL2 = tempNL;
                    tempNL = tempNL->next;
                    if (isWait)
                        evictWaitName(tempNL2->name);
                    else
                        evictDeskName(tempNL2->name);
                    continue;
                }
                if (tempNL != nullptr)
                    tempNL = tempNL->next;
            }
        }
    }

    void DOMAIN_EXPANSION()
    {
        domainexpansion();
        while (this->firstWaitingCustomer != nullptr && numDiningCustomers < MAXSIZE)
        {
            customer *tempcus = this->firstWaitingCustomer;
            this->firstWaitingCustomer = this->firstWaitingCustomer->next;
            this->numWaitingCustomers--;
            if ((this->numDiningCustomers < MAXSIZE / 2) || (MAXSIZE == 1 && this->numDiningCustomers == 0))
            {
                addDeskCus_1(tempcus);
            }
            else
            {
                addDeskCus_2(tempcus);
            }
        }
    }

    void LIGHT(int num)
    {
        customer *tempX = this->X;
        customer *tempWait = this->firstWaitingCustomer;
        if (num > 0)
            for (int i = 0; i < numDiningCustomers; i++)
            {
                tempX->print();
                tempX = tempX->next;
            }
        else if (num < 0)
            for (int i = 0; i < numDiningCustomers; i++)
            {
                tempX->print();
                tempX = tempX->prev;
            }
        else
            for (int i = 0; i < numWaitingCustomers; i++)
            {
                tempWait->print();
                tempWait = tempWait->next;
            }
    }
};