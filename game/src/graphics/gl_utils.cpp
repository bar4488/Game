#include "gl_utils.h"

unsigned int glutls::GetSizeOfType(unsigned int type) {
	switch (type) {
		case GL_FLOAT:
			return sizeof(float);
		case GL_UNSIGNED_INT:
			return sizeof(unsigned int);
		case GL_INT:
			return sizeof(int);
		case GL_UNSIGNED_BYTE:
			return sizeof(unsigned char);
		case GL_BYTE:
			return sizeof(char);
	}
}
