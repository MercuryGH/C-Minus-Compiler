void swapArrayElement(int arr[], int i, int j) {
    int tmp;

    tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

int getPartition(int arr[], int l, int r) {
    int ptr;
    int pivot;
    int i;
    int tmp;

    pivot = arr[l];
    ptr = l;  // ptr 始终指向最右边的 <= pivot 的数
    i = l + 1;
    while (i <= r) {
        if (arr[i] <= pivot) {
            ptr = ptr + 1;
            // swap
            swapArrayElement(arr, ptr, i);
        }
        i = i + 1;
    }
    swapArrayElement(arr, ptr, l);
    return ptr;
}

/**
 * @brief 为了方便做堆栈平衡，
 * C-Minus 的 return 语句是延迟的
 * 所以必须用完整的 if-else
 */
void quickSort(int arr[], int l, int r) {
    int pos;
    if (l >= r) {
        return;
    } else {
        pos = getPartition(arr, l, r);
        quickSort(arr, l, pos - 1);
        quickSort(arr, pos + 1, r);
    }
}

int main()
{
    int array[110];
    int i;
    int n;

    n = input();

    i = 0;
    while (i < n)
    {
        array[i] = input();
        i = i + 1;
    }

    quickSort(array, 0, n - 1);

    i = 0;
    while (i < n)
    {
        output(array[i]);
        i = i + 1;
    }
}
