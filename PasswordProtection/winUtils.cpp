#include "winUtils.h"


#include <Windows.h>

const size_t SHORT_SIZE = 16;
const SHORT PRESSED_MASK = 1 << 15;
//const SHORT RELEASED_MASK = PRESSED_MASK;

bool isKeyDownLogic(SHORT value);




bool WinUtils::is_global_key_down (int n_virt_key)
{
	SHORT state = GetAsyncKeyState(n_virt_key);


	return isKeyDownLogic(state);
}





bool isKeyDownLogic(SHORT value)
{
	return (PRESSED_MASK & value) == PRESSED_MASK;
}