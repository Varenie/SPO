#include <Windows.h>
#include <fstream>
#include <string>
#include <iostream>

using namespace std; 

#define NUMBER_OF_BYTES_IN_SECTOR_OFFSET 0x0B //�� ����� �������� ��������� ������ �������
#define NUMBER_OF_SECTORS_IN_SECTION_FIRST_OFFSET 0x13 //�� ����� �������� ��������� ���������� �������� ���� ��� ������ 65535
#define NUMBER_OF_SECTORS_IN_SECTION_SECOND_OFFSET 0x20 //�� ����� �������� ��������� ���������� ��������, ���� �� ������ 65535

string drivePath = "\\\\.\\D:"; 


UINT getNumberOfBytesInSector() {
	short int numberOfBytes; //��� ��� ������ ����� ����� 2 �����

	ifstream infile(drivePath.c_str(), ios::in | ios::binary); // ����������� � ����� ����� �����, � ���������, ��� �� ��������(in - ������� ��� ������)
	if (!infile) {
		cerr << "ERROR! Device input could not be set!\n";
	}

	infile.ignore(NUMBER_OF_BYTES_IN_SECTOR_OFFSET); //�� ������ �� ���� �������, ���� �� �������� ���� � �������� �������?

	infile.read((char*)&numberOfBytes, sizeof(short int)); //������ 2 ����� ����
	if (infile.gcount() == 0) {
		cerr << "Data read error!" << endl;
	}

	infile.close(); //stream close

	return (UINT)numberOfBytes;
}

UINT getNumberOfSectorsInSection() {
	short int numberOfSectors; //��� ��� ������ ����� ����� 2 �����
	UINT numberOfSectorsU;

	ifstream infile(drivePath.c_str(), ios::in | ios::binary); // ����������� � ����� ����� �����, � ���������, ��� �� ��������(in - ������� ��� ������)
	if (!infile) {
		cerr << "ERROR! Device input could not be set!\n";
	}

	infile.ignore(NUMBER_OF_SECTORS_IN_SECTION_FIRST_OFFSET); // ������� �� ������� ����� � ����������� ��������

	infile.read((char*)&numberOfSectors, sizeof(short int)); //��������� 2 ����� ����������
	if (infile.gcount() == 0) {
		cerr << "Data read error!" << endl;
	}

	infile.close(); //stream close

	if (numberOfSectors != 0) { // ���� � ������ ����� ���� �����, �� ������ ������ ����
		return (UINT)numberOfSectors;
	} else {
		ifstream infile(drivePath.c_str(), ios::in | ios::binary); // ����������� � ����� ����� �����, � ���������, ��� �� ��������(in - ������� ��� ������)
		if (!infile) {
			cerr << "ERROR! Device input could not be set!\n";
		}

		infile.ignore(NUMBER_OF_SECTORS_IN_SECTION_SECOND_OFFSET);// ������� �� ������� ����� � ����������� ��������

		infile.read((char*)&numberOfSectorsU, sizeof(UINT)); // ��������� 4 ����� ����������
		if (infile.gcount() == 0) {
			cerr << "Data read error!" << endl;
		}

		infile.close(); //stream close

		return numberOfSectorsU;
	}
}

void getSectorDump(UINT numberOfBytes, UINT sectorNumber) {
	BYTE* dump = new BYTE[numberOfBytes];

	ifstream infile(drivePath.c_str(), ios::in | ios::binary); // ����������� � ����� ����� �����, � ���������, ��� �� ��������(in - ������� ��� ������)
	if (!infile) {
		cerr << "ERROR! Device input could not be set!\n";
	}

	infile.ignore(static_cast<__int64>(numberOfBytes) * sectorNumber);

	infile.read((char*)dump, numberOfBytes);
	if (infile.gcount() == 0) {
		cerr << "Data read error!" << endl;
	}

	infile.close();

	for (int i = 0; i < numberOfBytes / 16; i++) {
		printf("%09X| ", numberOfBytes * sectorNumber + i * 16);
		for (int j = 0; j < 16; j++) {
			if (j == 8) {
				printf(" ");
			}
			printf("%02X ", dump[i * 16 + j]);
		}

		printf(" | ");

		for (int j = 0; j < 16; j++) {
			if (dump[i * 16 + j] != 0x0A && dump[i * 16 + j] != 0x0D && dump[i * 16 + j] != 0x09) {// �������� �� ��������� ������� �� ������� ASCII
				printf("%c", dump[i * 16 + j]);
			}
		}
		printf("\n");
	}
}

int main() {
	UINT numberOfBytes;
	UINT numberOfSectors;

	numberOfBytes = getNumberOfBytesInSector();
	numberOfSectors = getNumberOfSectorsInSection();

	cout << "Number of bytes in sector: " << numberOfBytes << endl;
	cout << "Number of sectors in  section: " << numberOfSectors << endl;

	getSectorDump(numberOfBytes, 0);
}