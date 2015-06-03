//
// Created by dsklyadnev on 6/2/15.
//

#ifndef MQTTBROKER_SINGLETON_H
#define MQTTBROKER_SINGLETON_H

template <class T>
class Singleton
{
public:
    /// Function returns object of class T
    /**
     * Function creates object when called for the first time
     * @return Single object of class T
    */
    static T& instance()
    {
        static T impl;
        return impl;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton()
    {}

    virtual ~Singleton()
    {}
};



#endif //MQTTBROKER_SINGLETON_H
