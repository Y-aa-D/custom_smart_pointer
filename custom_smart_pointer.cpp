#include <iostream>

template <typename Resource>
class WeakPtr;

template <typename Resource>
class SharedPtr
{
public:
    SharedPtr() : resource(nullptr), referenceCount(nullptr) {}

    explicit SharedPtr(Resource* res) : resource(res), referenceCount(new long(1)) {}

    SharedPtr(const SharedPtr<Resource>& rhs) : resource(rhs.resource), referenceCount(rhs.referenceCount)
    {
        IncrementReferenceCount();
    }

    SharedPtr(const WeakPtr<Resource>& rhs) : resource(rhs.resource), referenceCount(rhs.referenceCount)
    {
        IncrementReferenceCount();
    }

    SharedPtr<Resource>& operator=(const SharedPtr<Resource>& rhs)
    {
        if (this != &rhs)
        {
            DecrementReferenceCount();
            resource = rhs.resource;
            referenceCount = rhs.referenceCount;
            IncrementReferenceCount();
        }
        return *this;
    }

    ~SharedPtr()
    {
        DecrementReferenceCount();
    }

    void Reset()
    {
        DecrementReferenceCount();
        resource = nullptr;
        referenceCount = nullptr;
    }

    void Reset(Resource* res)
    {
        DecrementReferenceCount();
        resource = res;
        referenceCount = new long(1);
    }

    void Swap(SharedPtr<Resource>& rhs)
    {
        std::swap(resource, rhs.resource);
        std::swap(referenceCount, rhs.referenceCount);
    }

    Resource* Get() const
    {
        return resource;
    }

    Resource& operator*() const
    {
        return *resource;
    }

    Resource* operator->() const
    {
        return resource;
    }

    long UseCount() const
    {
        if (referenceCount != nullptr)
        {
            return *referenceCount;
        }
        return 0;
    }

private:
    Resource* resource;
    long* referenceCount;

    template <typename Resource>
    friend class WeakPtr;

    void IncrementReferenceCount()
    {
        if (referenceCount != nullptr)
        {
            (*referenceCount)++;
        }
    }

    void DecrementReferenceCount()
    {
        if (referenceCount != nullptr)
        {
            (*referenceCount)--;
            if (*referenceCount == 0)
            {
                delete resource;
                delete referenceCount;
            }
        }
    }
};

template <typename Resource>
class WeakPtr
{
public:
    WeakPtr() : resource(nullptr), referenceCount(nullptr) {}

    WeakPtr(const WeakPtr<Resource>& rhs) : resource(rhs.resource), referenceCount(rhs.referenceCount) {}

    WeakPtr(const SharedPtr<Resource>& rhs) : resource(rhs.Get()), referenceCount(rhs.referenceCount) {}

    WeakPtr<Resource>& operator=(const WeakPtr<Resource>& rhs)
    {
        if (this != &rhs)
        {
            resource = rhs.resource;
            referenceCount = rhs.referenceCount;
        }
        return *this;
    }

    WeakPtr<Resource>& operator=(const SharedPtr<Resource>& rhs)
    {
        resource = rhs.Get();
        referenceCount = rhs.referenceCount;
        return *this;
    }

    ~WeakPtr() {}

    void Reset()
    {
        resource = nullptr;
        referenceCount = nullptr;
    }

    void Swap(WeakPtr<Resource>& rhs)
    {
        std::swap(resource, rhs.resource);
        std::swap(referenceCount, rhs.referenceCount);
    }

    long UseCount() const
    {
        if (referenceCount != nullptr)
        {
            return *referenceCount;
        }
        return 0;
    }

    bool Expired() const
    {
        return (referenceCount == nullptr || *referenceCount == 0);
    }

    SharedPtr<Resource> Lock() const
    {
        return SharedPtr<Resource>(*this);
    }

private:
    Resource* resource;
    long* referenceCount;

    template <typename Resource>
    friend class SharedPtr;
};

int main()
{
    SharedPtr<int> ptr1(new int(5));
    SharedPtr<int> ptr2(ptr1);

    std::cout << *ptr1 << std::endl;
    std::cout << *ptr2 << std::endl;

    *ptr2 = 10;

    std::cout << *ptr1 << std::endl;
    std::cout << *ptr2 << std::endl;

    //---------------------------------------------------------------------

    SharedPtr<int> ptr(new int(5));
    WeakPtr<int> weakPtr(ptr);

    if (!weakPtr.Expired()) {
        SharedPtr<int> lockedPtr = weakPtr.Lock();
        std::cout << *lockedPtr << std::endl;
    }

    ptr.Reset();

    if (weakPtr.Expired()) {
        std::cout << "Weak pointer expired" << std::endl;
    }

    return 0;
}
