int globalNumArray[110];

void swapArrayElement(int arr[], int i, int j) {
    int tmp;

    tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

int get_minimum_index(int arr[], int l, int r)
{
    int min_index;
    int min_val;
    int i;

    min_index = l;
    min_val = arr[l];
    i = l + 1;

    while (i <= r) {
        if (arr[i] < min_val) {
            min_val = arr[i];
            min_index = i;
        }
        i = i + 1;
    }
    return min_index;
}

void selection_sort(int arr[], int l, int r) 
{
    int i;
    int min_index;
    int tmp;

    i = l;
    while (i <= r) {
        min_index = get_minimum_index(arr, i, r);

        // swap
        /*
            tmp = arr[min_index];
            arr[min_index] = arr[i];
            arr[i] = tmp;
        */
        swapArrayElement(arr, min_index, i);
        i = i + 1;
    }
}

int main()
{
    int i;
    int n;

    n = input();

    i = 0;
    while (i < n)
    {
        globalNumArray[i] = input();
        i = i + 1;
    }

    selection_sort(globalNumArray, 0, n - 1);

    i = 0;
    while (i < n)
    {
        output(globalNumArray[i]);
        i = i + 1;
    }
}
