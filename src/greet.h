#ifndef MYAPP_GREET_H
#define MYAPP_GREET_H

#include <string>

namespace myapp {

/**
 * @brief 名前を受け取り挨拶文字列を返す。
 * @param name  挨拶する相手の名前。
 * @return "Hello, {name}!" 形式の文字列。
 */
std::string Greet(const std::string& name);

}  // namespace myapp

#endif  // MYAPP_GREET_H
