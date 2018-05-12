#include "MainForm.h"

[kame::STAThreadAttribute]
int main(int argc, char *argv[])
{
	kame::MainForm ^fm = gcnew kame::MainForm();

	fm->ShowDialog();

	return 0;
}