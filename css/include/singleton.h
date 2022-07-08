#ifndef SINGLETON_H
#define SINGLETON_H

#include <QMutex>
#include <QScopedPointer>

/**
 * 使用方法:
 * 1. 定义类为单例:
 *     class ConnectionPool {
 *         SINGLETON(ConnectionPool) // Here
 *     public:
 *
 * 2. 获取单例类的对象:
 *     Singleton<ConnectionPool>::getInstance();
 *     ConnectionPool *pool = Singleton<ConnectionPool>::getInstance();
 * 注意: 如果单例的类需要释放的资源和 Qt 底层的信号系统有关系，例如 QSettings，QSqlDatabase 等，
 *     需要在程序结束前手动释放(也就是在 main() 函数返回前调用释放资源的函数，参考 ConnectionPool 的调用)，
 *     否则有可能在程序退出时报系统底层的信号错误，导致如 QSettings 的数据没有保存。
 */
template <typename T>
class Singleton {
public:
    static T* getInstance();
};

/*-----------------------------------------------------------------------------|
 |                          Singleton implementation                           |
 |----------------------------------------------------------------------------*/

template <typename T>
T* Singleton<T>::getInstance() {
    static T instance;
    return &instance;
}

/*-----------------------------------------------------------------------------|
 |                               Singleton Macro                               |
 |----------------------------------------------------------------------------*/
#define SINGLETON(Class)                                 \
private:                                                 \
    Class();                                             \
    ~Class();                                            \
    Class(const Class &other) = delete;                  \
    Class& operator=(const Class &other) = delete;       \
    friend class Singleton<Class>;                       \
    friend struct QScopedPointerDeleter<Class>;

#endif // SINGLETON_H
