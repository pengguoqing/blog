#include<iostream>
#include<array>
#include<optional>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<atomic>

template<class T> struct IsArray
{
   static bool m_value{false};
};

template<class T> struct IsArray<T[]>
{
    static bool m_value{true};
};

template<class T, size_t N> struct IsArray<T[N]>
{
     static bool m_value{true};
};


template<class T> 
bool CheckIsArray(const T&)
{
    return false;
}

template<class T, size_t N> 
bool CheckIsArray(const T(&)[N] )
{
    return true;
}

//test
int array[6];
int value;
CheckIsArray(array); //return true
CheckIsArray(value); //return false


template<class T = void>
struct less
{
    constexpr bool operator()(const T& lhs, const T& rhs) const 
    {
        return lhs < rhs; 
    }
};

template<>
struct less<void>
{
    template<class T, class U>
    auto operator()(const T&& lhs, const U&& rhs) const 
    {
        return std::forward<T>(lhs) < std::forward<T>(rhs); 
    }

    typedef void is_transparent;
};

#include<map>
#include<string>
std::map<std::string, int> name_age_mp;

template <typename IdType>
struct id_compare 
{
    template <typename T, typename U>,
    bool operator()(const T& lhs, const U& rhs) const  { return lhs.id < rhs.id; },
    template <typename T>,
    bool operator()(const T& lhs, IdType rhs_id) const { return lhs.id < rhs_id; },
    template <typename T>,
    bool operator()(IdType lhs_id, const T& rhs) const { return lhs_id < rhs.id; },

    typedef void is_transparent;

};

struct Obj
{
    int id;
    const char* name;
};

std::set<Obj, id_compare<int>> s{{0, "zero"}, {1, "one"}, {2, "two"}};

auto less_obj = [](auto&& lhs, auto&& rhs)
{
    return std::forward<decltype(lhs)>(lhs) < std::forward<decltype(rhs)>(rhs);
};

class BaseObj
{
public:
    void BufferOp();
};

class DerivedObj : private BaseObj
{
public:
    using BaseObj::BufferOp;
};

template <int n>
struct Factorial
{
    static const int m_value = Factorial<n> * Factorial<n-1>::m_value;
};


template <>
struct Factorial<0>
{
    static const int m_value = 1;
};

//假如直接 std::cout<< Factorial<5> <<std::endl; 那么将直接输出 120   

template<class T>
struct RemoveConst
{
    using type = T;
};

template<class T>
struct RemoveConst<const T>
{
    using type = T;
};

template<class T>
using RemoveConstT = typename RemoveConst<T>::type;

struct A
{
    A(){std::cout<<"create obj"<<std::endl;}
    ~A(){std::cout<<"destory obj"<<std::endl;}
    A(const A& that){std::cout<<"copy create obj"<<std::endl;}
    A(A&& that){std::cout<<"move create obj"<<std::endl;}    
};

A GetObj()
{
    A a;
    return a;
}

  
A GetObjMove()
{
    A a1;
    A a2;
    if (rand()> 100){
        return a1;
    }else{
        return a2;
    }
}

int main()
{
    auto a = GetObj();
    return 0;
}

enum class DivError:int
{
    kSuccess,
    kInvalidParam,
    kDivideOverFlows,
};

template<>
struct std::is_error_code_enum<DivError>: true_type {};

class DivErrorCategory:public std::error_category
{
public:
    const char* name()const noexcept override 
    {
        return "Divide Error";
    }

    std::string message(int code) const override
    {
        switch (static_cast<DivError>(code))
        {
        case DivError::kSuccess:
            return "Success";
        
        case DivError::kInvalidParam:
            return "InvalidParam";
        
        case DivError::kDivideOverFlows:
            return "DivideOverFlow";
        }
         return "Unknown";
    }

    std::error_condition default_error_condition(int code) const noexcept override
    {
        switch (static_cast<DivError>(code))
        {
        case DivError::kSuccess:
            break;
        
        case DivError::kInvalidParam:
            return std::make_error_condition(std::errc::invalid_argument);

        case DivError::kDivideOverFlows:
            return std::make_error_condition(std::errc::value_too_large);  
        }

        return std::error_condition(code, *this);
    }   
};

std::error_code make_error_code(DivError e)
{
    static DivErrorCategory diverror_category;
    return {static_cast<int>(e), diverror_category};
}

struct BigObj
{
    BigObj(){std::cout<<"Big Obj Construct"<<std::endl;}
    ~BigObj(){std::cout<<"Big Obj deConstruct"<<std::endl;}
    BigObj(const BigObj& that){std::cout<<"Big Obj copy"<<std::endl;}
    void Op(){std::cout<<"Big Obj Op"<<std::endl;}
private:
   std::array<int, 1024> m_big_data;
};

BigObj GetBigObj(bool flag)
{
    if(!flag){
        std::runtime_error("BigObj bad flag");
    }
    BigObj obj;
    obj.Op();
    return obj;
}

std::optional<BigObj> GetBigObj(bool flag)
{
    if(!flag){
        return std::nullopt;
    }

    BigObj obj;
    obj.Op();
    return obj;
}

std::optional<BigObj> GetBigObj(bool flag)
{
    std::optional<BigObj> obj;
    if(!flag){
        return obj;
    }

    obj.emplace();
    obj->Op();
    return obj;
}

std::condition_variable_any cv;

void WorkFunc(int& result, std::condition_variable_any& cv, std::mutex& cv_mut, bool& ready_flag)
{
    result = 100;
    {
        std::unique_lock w_lock(cv_mut);
        ready_flag = true;
    }
    
    cv.notify_one();
    return;
}

int main()
{
    std::condition_variable_any cv;
    std::mutex cv_mut;
    bool ready_flag{false};
    int result{0};

    std::thread workth(std::ref(result), std::ref(cv), std::ref(cv_mut), std::ref(ready_flag));
    std::unique_lock r_lock(cv_mut);
    cv.wait(r_lock, [&ready_flag](){return ready_flag;});
    std::cout<<"result is:"<< result <<std::endl;

    if (workth.joinable()){
        workth.join();
    }
    
    return 0;
}
	
    static T* InitInstance(Args&&... args)
	{
		if (nullptr == ins) {
			std::lock_guard<std::mutex> w_lock(m_mutex);
			if (nullptr == ins) {
				ins = new T(std::forward<Args>(args)...);	
			}
		}
    }

        static T* InitInstance(Args&&... args)
	{
		T* ins = m_instance.load(std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_acquire);
		if (nullptr == ins)
		{
			std::lock_guard<std::mutex> w_lock(m_mutex);
			ins = m_instance.load(std::memory_order_relaxed);
			if (nullptr == ins) {
				ins = new T(std::forward<Args>(args)...);
				std::atomic_thread_fence(std::memory_order_release);
				m_instance.store(ins, std::memory_order_relaxed);
			}
		}
    }