#include"stdafx.h"
#include"log.h"

class log* log::m_pInstance = NULL;

void Log(const char* text, int code) {
	log::GetInstance()->log_error(text, code);
}