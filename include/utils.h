#ifndef UTILS_H
#define UTILS_H

#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }
#define SAFE_ARRAY_DELETE(x) if(x) { delete[] x; x = nullptr; }

#endif // UTILS_H
