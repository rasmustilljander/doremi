#pragma once
#include <unordered_map>
#include <type_traits>

enum class WrappedType
{
    INT,
    STRING,
    FLOAT,
    BOOL
};

template <typename T> struct WrapperContainer
{
    WrapperContainer(const T& _data)
    {
        if(std::is_same<T, int>::value)
        {
            type = WrappedType::INT;
        }
        else if(std::is_same<T, std::string>::value)
        {
            type = WrappedType::STRING;
        }
        else if(std::is_same<T, float>::value)
        {
            type = WrappedType::FLOAT;
        }
        else if(std::is_same<T, bool>::value)
        {
            type = WrappedType::BOOL;
        }
        data = new T(_data);
    }

    ~WrapperContainer() { delete data; }

    T* data;
    WrappedType type;
};

class VariableManager
{
public:
    VariableManager(VariableManager const&) = delete;
    VariableManager& operator=(const VariableManager&) = delete;

    static VariableManager& GetVariablesManager()
    {
        static VariableManager variableManager;
        return variableManager;
    }

    template <typename T> const T& GetValue(const std::string& key)
    {
        if(m_values.count(key) == 1) return *((WrapperContainer<T>*)m_values[key])->data;
        const std::string message = std::string("No key with the given name exists: ") + key;
        throw std::exception(message.c_str());
    }

    template <typename T> void SetValue(const std::string& key, const T& value)
    {
        WrapperContainer<T>* container = new WrapperContainer<T>(value);

        if(m_values.count(key) == 1) delete m_values[key];
        m_values[key] = (void*)container;
    }

    const std::unordered_map<std::string, void*>& GetAllValues() const { return m_values; }

private:
    VariableManager() {}
    virtual ~VariableManager() {}
    std::unordered_map<std::string, void*> m_values;
};