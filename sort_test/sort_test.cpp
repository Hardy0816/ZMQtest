#include<iostream>
#include<string>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
using namespace std;

void sort_Bubble(int data[], int n)
{
	int a = 0;
    for(int i = 0;i < n-1;i++)
	{
		for(int j = 0;j < n-1; j++)
		{
			if(data[j] > data[j+1])
			{
				a = data[j+1];
				data[j+1] = data[j];
				data[j] = a;
			}
		}
	}
}

void sort_Cocktail(int data[], int n)
{
	int mark = 1;
	int a;
	while(mark != 0)
	{
		mark = 0;
		//forward 
		for(int i = 0;i < n-1; i++)
		{
			if(data[i] > data[i+1])
			{
				a = data[i+1];
				data[i+1] = data[i];
				data[i] = a;
				mark++;
			}
		}
		//backward
		for(int j = n-2;j > 0; j--)
		{
			if(data[j] > data[j+1])
			{
				a = data[j+1];
				data[j+1] = data[j];
				data[j] = a;
				mark++;
			}
		}
	}
}

void sort_Insertion(int data[], int n)
{
	int a = 0;
	for(int i = 1;i < n; i++)
	{
		for(int j = 0;j < i;j++)
		{
			if(data[i] < data[j])
			{
				a = data[i];
				for(int k = i;k > j;k--)
				{
					data[k] = data[k-1];
				}
				data[j] = a;
			}
		}
	}
}

void sort_Bucket(int data[], int n)
{
	int* Bucket_base = new int[n];
	for(int i = 0;i < n;i++)
	{
		Bucket_base[data[i]] = 1;
	}
	int k = 0;
	for(int j = 0;j < 256; j++)
		if(Bucket_base[j] == 1)
			{
				data[k] = j;
				k++;
			}
		delete[] Bucket_base;
}

void sort_Counting(int data[], int n)
{
	int* Count_Bucket = new int[n];
	for(int i = 0;i < n; i++)
	{
		int k = 0;
		for(int j = 0;j < n; j++)
		{
			if(data[i]>data[j])
				k++;
		}
		Count_Bucket[k] = data[i];
	}
	for(int i = 0;i < n; i++)
	{
		data[i] = Count_Bucket[i];
	}
	delete[] Count_Bucket;
}

void sort_Merge_s(int data_n[],int n,int data_m[],int m,int data[])
{
	int i = 0,j = 0,k = 0;
	for(;i < m && j < n;)
	{
		if(data_m[i] < data_n[j])
		{
			data[k] = data_m[i];
			k++;
			i++;
		}
		else
		{
			data[k] = data_n[j];
			k++;
			j++;
		}
	}
	for(;i < m;k++,i++)
		data[k] = data_m[i];
	for(;j < n;k++,j++)
		data[k] = data_n[j];
}

void sort_Merge(int data[], int n)
{
	int a = 0;
	for(int i = 0;i < n/2;i++)
	{
		if(data[i*2]>data[i*2+1])
		{
			a = data[i*2];
			data[i*2] = data[i*2+1];
			data[i*2+1] = a;
		}
	}

	int* data_m = new int[n];
    int p1 = 0,p2 = 0, k =2, p = 0;
	int q1 = 0, q2 =0;
	for(;k < n;k = k * 2)
	{
		for(int p = 0;p < n;p = p+k*2)
		{
			q1 = 0; q2 = 0;
            p1 = p; p2 = p1 + k ;
			for(; q1< k && q2 < k && p+ q1 < n && p2 + q2 < n ;)
			{
				//q1 = p1; q2 = p2;
				if(data[p1+q1] < data[p2+q2])
				{
					data_m[p+q1+q2] = data[p1+q1];
					q1++;
				}
				else
				{
					data_m[p+q1+q2] = data[p2+q2];
					q2++;
				}
			}
			for(;q1 < k && p + q1 < n;)
			{
				data_m[p+q1+q2] = data[p1+q1];
				q1++;
			}
			for(;q2 < k && p + q1+ q2 < n;)
			{
				data_m[p+q1+q2] = data[p2+q2];
				q2++;
			}	

		}
		for(int j = 0;j < n;j++)
			data[j] = data_m[j];
	}
	delete[] data_m;
}

void sort_Heap(int data[],int n)
{
	int a = 0, i = 0;
    int* data_m = new int[n];
	for(int j = 0; j < n;j++)
	{
		
		while(1)
		{
			int mark = 1;
			for(i = 0; 2*i < n ;i++)
			{
				if(2*i < n)
				{
					if(data[i]<data[2*i+1])
					{
						a = data[i];
						data[i] = data[2*i+1];
						data[2*i+1] = a;
						mark = 0;
					}
				}
				if(2*i + 2 < n)
				{
					if(data[i]<data[2*i+2])
					{
						a = data[i];
						data[i] = data[2*i+2];
						data[2*i+2] = a;
						mark = 0;
					}
				}
			}
			if(mark == 1)
				break;
		}

			//for(i = 0;i < n;i++)  
			//cout << data[i] << " ";
			//cout << endl;

			data_m[j] = data[0];
			for(int k = 0;k < n;k++)
				data[k] = data[k+1];
		}

	for(int k = 0;k < n;k++)
		data[k] = data_m[k];
	delete[] data_m;
}

int main()
{
	int a[10] = {7,3,5,4,8,82,10,6,1,9};
	cout << "Before sort:" << endl;
	int a_length = sizeof(a)/sizeof(a[0]);
	for(int i = 0;i < a_length;i++)  
    cout << a[i] << " ";
	cout << endl;

	//sort_Bubble(a, a_length);
	//sort_Cocktail(a, a_length);
	//sort_Insertion(a, a_length);
	//sort_Bucket(a, a_length);
	//sort_Counting(a, a_length);
	//sort_Merge(a, a_length);
	sort_Heap(a,a_length);
	cout << "After sort:" << endl;
	for(int i = 0;i < a_length;i++)  
    cout << a[i] << " ";
	cout << endl;

	return 0;
}