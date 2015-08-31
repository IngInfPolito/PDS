#include "FGrep.h"

/*
 * This code is only for Windows.
 */

int _tmain(int argc, TCHAR* argv[]) {

	if (argc != 4) {
		_tprintf(TEXT("Usage: %s <dir> <text> <output_file>\n"), argv[0]);
		return -1;
	}

	{
		ResultFile rf(argv[3]);

		FGrep fg(20, &rf);
		if (fg.execute(argv[1], argv[2])) {
			_tprintf(TEXT("System error!\n"));
			return -2;
		}

		if (rf.fail()) {
			_tprintf(TEXT("Can't write to '%s'!\n"), argv[3]);
			return -3;
		}
	} // Destroy fg and rf (Close output file)

	_tprintf(TEXT("Finished! Output redirected to '%s'.\n"), argv[3]);
	return 0;
}
