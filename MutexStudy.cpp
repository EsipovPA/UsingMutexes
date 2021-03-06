// MutexStudy.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

using namespace std;

class MyTestClass 
{
private:
	int firstVal;
	int secondVal;

	const int requiredCallsNum = 4;
	int callCount = 0;

	HANDLE hMutex;

	void ChangeVal(int valNum, bool increseVal) {
		WaitForSingleObject(this->hMutex, INFINITE);

		callCount++;

		cout << "\n**********\nThread " << this_thread::get_id() << " have captured the mutex\n";

		int *val = NULL;
		if (valNum == 1) { val = &this->firstVal; }
		if (valNum == 2) { val = &this->secondVal; }
		
		cout << "Thread (id) " << this_thread::get_id() << " ";
		if (increseVal) { cout << "increses"; }
		else { cout << "decreses"; }
		cout << " " << valNum << "-th val\n";

		for (int i = 0; i < 10; i++) {
			if (increseVal) { *val += 1; } else { *val -= 1; }
			cout << "/		Current " << valNum << "-th val =" << *val << "\n";
			this_thread::sleep_for(chrono::milliseconds(100));
		}

		cout << "Thread " << this_thread::get_id() << " have released the mutex\n**********\n";

		ReleaseMutex(this->hMutex);
	}

public:

	//Перегрузим конструкторы
	MyTestClass() {
		this->hMutex = CreateMutex(NULL, false, NULL);
		ReleaseMutex(hMutex);
	}
	MyTestClass(int valOne, int valTwo) : MyTestClass()
	{
		this->firstVal = valOne;
		this->secondVal = valTwo;
	}

	//Изменение значений первой переменной
	void IncreseFirst() { this->ChangeVal(1, true); }
	void DecreseFirst() { this->ChangeVal(1, false); }

	//Изменение значений второй переменной
	void IncreseSecond() { this->ChangeVal(2, true); }
	void DecreseSecond() { this->ChangeVal(2, false); }

	//Получение доступа к значениям
	int GetValByNum(int num) {
		int outData;
		WaitForSingleObject(this->hMutex, INFINITE);
		if (num == 1) { outData = this->firstVal; }
		if (num == 2) { outData = this->secondVal; }
		if (num != 1 && num != 2) {
			outData = NULL;
			cout << "In the GetValByNum\n" << num << " is not a known value number\n";
		}
		ReleaseMutex(hMutex);
		return outData;
	}

	//Проверим количество обращений
	bool AllCallsAreDone() {
		bool isDone = false;
		WaitForSingleObject(hMutex, INFINITE);
		cout << "Checking Calls count\n";
		if (this->callCount == this->requiredCallsNum){
			cout << "Calls count is normal\n";
			isDone = true;
		}
		else {
			cout << "Calls count is too small\n";
		}
		ReleaseMutex(hMutex);
		return isDone;
	}
};

void CallClassFunc(void (*funcPointer)())
{
	cout << "Have to call method from \"Interface\" function through the function-pointer\n";
	(*funcPointer)();
	cout << "End of the \"Interface\" function\n";
}

void CallIncreaseFirst(MyTestClass *objPointer)
{
	cout << "Call IncreaseFirst from additional func\n";
	objPointer->IncreseFirst();
	cout << "End of the additional func\n";
}

void CallIncreaseSecond(MyTestClass *objPointer)
{
	cout << "Call IncreaseSecond from additional func\n";
	objPointer->IncreseSecond();
	cout << "End of the additional finc\n";
}

void CallDecreseFirst(MyTestClass *objPointer)
{
	objPointer->DecreseFirst();
}

void CallDecreseSecond(MyTestClass *objPointer) {
	objPointer->DecreseSecond();
}

int main()
{
	//Создадим дескрипторы для сторонних потоков
	HANDLE hFirstThread;
	HANDLE hSecndThread;
	HANDLE hThirdThread;
	HANDLE hForthThread;

	//MyTestClass testObj = MyTestClass(1, 1);
	MyTestClass *testObjPointer = new MyTestClass(1, 1);


	cout << "In the main thread [id: " << this_thread::get_id() << "]\nLet's create MyTestClass object\n";
	cout << "\nNow let's create 4 threads to increse first val of testObj\n";
	
	cout << "\n*******\n";

	hFirstThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CallIncreaseFirst, testObjPointer, 0, 0);
	hSecndThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CallIncreaseSecond, testObjPointer, 0, 0);
	hThirdThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CallDecreseFirst, testObjPointer, 0, 0);
	hForthThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CallDecreseSecond, testObjPointer, 0, 0);


	cout << "Let's try to get acess to the values\n";

	bool allCallsAreDone;
	do {
		allCallsAreDone = testObjPointer->AllCallsAreDone();

		if (allCallsAreDone) {
			cout << "First value = " << testObjPointer->GetValByNum(1) << "\n";
			cout << "Second value = " << testObjPointer->GetValByNum(2) << "\n";
		}
		else {
			this_thread::sleep_for(chrono::seconds(5));
		}
	} while (!allCallsAreDone);

	cout << "\n*******\n";

	cout << "Press any key\n";
	cin.get();
    return 0;
}