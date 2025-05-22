#include <iostream>
#include <stack>
#include <algorithm>
#include <string.h>

const int MIN_MERGE = 32;
int min_gallop = 7;

int calcMinRun(int n)
{
    int r = 0;
    while (n >= MIN_MERGE)
    {
        r |= (n & 1);
        n >>= 1;
    }
    return n + r;
}

void insertionSort(int *arr, int left, int right)
{
    for (int i = left + 1; i <= right; ++i)
    {
        int val = arr[i], j = i - 1;
        while (j >= left && arr[j] > val)
        {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = val;
    }
}

int gallopLeft(int x, int *base, int len)
{
    int ofs = 1, lastOfs = 0;

    if (x > base[0])
    {
        while (ofs < len && x > base[ofs])
        {
            lastOfs = ofs;
            ofs = (ofs << 1) + 1;
        }
        if (ofs > len)
            ofs = len;
        lastOfs++;
    }
    else
    {
        return 0;
    }

    int low = lastOfs, high = ofs;
    while (low < high)
    {
        int mid = low + (high - low) / 2;
        if (x > base[mid])
            low = mid + 1;
        else
            high = mid;
    }
    return low;
}

int gallopRight(int x, int *base, int len)
{
    int ofs = 1, lastOfs = 0;

    if (x < base[0])
    {
        return 0;
    }
    else
    {
        while (ofs < len && x >= base[ofs])
        {
            lastOfs = ofs;
            ofs = (ofs << 1) + 1;
        }
        if (ofs > len)
            ofs = len;
        lastOfs++;
    }

    int low = lastOfs, high = ofs;
    while (low < high)
    {
        int mid = low + (high - low) / 2;
        if (x >= base[mid])
            low = mid + 1;
        else
            high = mid;
    }
    return low;
}

void mergeAt(int *arr, Run a, Run b)
{
    int *temp = new int[a.length];
    memcpy(temp, arr + a.start, a.length * sizeof(int));

    int i = 0, j = b.start, dest = a.start;
    int aLen = a.length, bLen = b.length;

    int countA = 0, countB = 0;

    while (i < aLen && j < b.start + bLen)
    {
        if (temp[i] <= arr[j])
        {
            arr[dest++] = temp[i++];
            countA++;
            countB = 0;
        }
        else
        {
            arr[dest++] = arr[j++];
            countB++;
            countA = 0;
        }

        if ((countA | countB) >= min_gallop)
        {
            int gallopA = gallopRight(arr[j], temp + i, aLen - i);
            memcpy(arr + dest, temp + i, gallopA * sizeof(int));
            dest += gallopA;
            i += gallopA;

            if (i >= aLen)
                break;

            int gallopB = gallopLeft(temp[i], arr + j, b.start + bLen - j);
            memcpy(arr + dest, arr + j, gallopB * sizeof(int));
            dest += gallopB;
            j += gallopB;

            if (j >= b.start + bLen)
                break;

            min_gallop += 1;
        }
    }

    while (i < aLen)
        arr[dest++] = temp[i++];

    delete[] temp;
}

void mergeCollapse(stack<Run> &runs, int *arr)
{
    while (runs.size() > 1)
    {
        Run X = runs.top();
        runs.pop();
        Run Y = runs.top();
        runs.pop();

        Run Z;
        if (!runs.empty())
        {
            Z = runs.top();
            runs.pop();
            if (Z.length <= Y.length + X.length)
            {
                if (Z.length < X.length)
                {
                    runs.push(Y);
                    mergeAt(arr, Z, Y);
                    Z.length += Y.length;
                    runs.push(Z);
                }
                else
                {
                    mergeAt(arr, Y, X);
                    Y.length += X.length;
                    runs.push(Z);
                    runs.push(Y);
                }
                continue;
            }
            else
            {
                runs.push(Z);
            }
        }

        if (Y.length <= X.length)
        {
            mergeAt(arr, Y, X);
            Y.length += X.length;
            runs.push(Y);
        }
        else
        {
            runs.push(Y);
            runs.push(X);
            break;
        }
    }
}

void TimSort(int *arr, int n)
{
    int minRun = calcMinRun(n);
    stack<Run> runStack;

    int i = 0;
    while (i < n)
    {
        int runStart = i;
        int runEnd = i + 1;

        if (runEnd < n && arr[runEnd] < arr[runStart])
        {
            while (runEnd < n && arr[runEnd] < arr[runEnd - 1])
                runEnd++;
            reverse(arr + runStart, arr + runEnd);
        }
        else
        {
            while (runEnd < n && arr[runEnd] >= arr[runEnd - 1])
                runEnd++;
        }

        int runLen = runEnd - runStart;
        if (runLen < minRun)
        {
            int end = min(runStart + minRun, n);
            insertionSort(arr, runStart, end - 1);
            runEnd = end;
            runLen = runEnd - runStart;
        }

        runStack.push({runStart, runLen});
        mergeCollapse(runStack, arr);

        i = runEnd;
    }

    while (runStack.size() > 1)
    {
        Run X = runStack.top();
        runStack.pop();
        Run Y = runStack.top();
        runStack.pop();
        mergeAt(arr, Y, X);
        Y.length += X.length;
        runStack.push(Y);
    }
}




