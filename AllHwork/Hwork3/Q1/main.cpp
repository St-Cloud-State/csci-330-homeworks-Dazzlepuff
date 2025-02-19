#include <iostream>
#include <vector>
#include <stack>

// Partition function
int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high]; // Pivot element
    int i = low - 1; // Index of smaller element
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Iterative QuickSort function
void quickSort(std::vector<int>& arr) {
    std::stack<std::pair<int, int>> stack;
    stack.push({0, arr.size() - 1});
    
    while (!stack.empty()) {
        int low = stack.top().first;
        int high = stack.top().second;
        stack.pop();
        
        if (low < high) {
            int pivot = partition(arr, low, high);
            
            // Push left side to stack
            if (pivot - 1 > low) {
                stack.push({low, pivot - 1});
            }
            
            // Push right side to stack
            if (pivot + 1 < high) {
                stack.push({pivot + 1, high});
            }
        }
    }
}

int main() {
    std::vector<int> arr = {10, 7, 8, 9, 1, 5};
    quickSort(arr);
    
    std::cout << "Sorted array: ";
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    return 0;
}