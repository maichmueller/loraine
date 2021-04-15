
#ifndef LORAINE_TYPES_H
#define LORAINE_TYPES_H

#include <cinttypes>
#include <memory>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// short handles for most common pointer types
template < typename T >
using uptr = std::unique_ptr< T >;
template < typename T >
using sptr = std::shared_ptr< T >;
template < typename T >
using wptr = std::weak_ptr< T >;
// a symmetric array to be used to store team symmetric concepts
template < typename T >
using SymArr = std::array< T, 2 >;

/// Code to enable classes to have fully correct std::shared_ptr clone methods using CRTP and
/// inheritance. This convoluted pattern is needed since std::shared_ptr<Base> and
/// std::shared_ptr<EventT> are not covariant types and thus cannot be used to simply chagne the
/// clone method's return type to std::shared_ptr<EventT> in every child class.
/// As such, any cloneable class needs to inherit from this pattern in the following way:
///
/// class Base : Cloneable<Base> {...};
/// class Child : Cloneable<Child, Base> {...};
/// class GrandChild : Cloneable<GrandChild, Child> {...};
///
/// If the constructors are supposed to be passed down, then the Child will need to inherit like
/// this:
/// class Child : Cloneable<Child, inherit_constructors<Base>> {
///     public:
///         using Cloneable::Cloneable;
///     ...
/// };

template < typename T >
class abstract_method {
};

template < typename T >
class virtual_inherit_from: virtual public T {
   using T::T;
};

/**
 * Helper struct to enable constructor inheritance in nested inheritance chains. This is helpful in
 * situations in which a CRTP adds functionality to a class, yet needs to decide the inheritance
 * itself, in order to not create ambiguities in the code, i.e.
 *      template <typename... Ts>
 *      class CRTPMixinClass : Ts... {...};
 *      class A : public CRTPMixinClass<A, BaseOfA> {...};
 * In such a scenario, this helper struct will make sure that CRTPMixinClass enables A to use
 * the constructors of BaseOfA or even simply forward them via a using statement of
 * using Cloneable<...>::Cloneable.
 */
template < typename Head, typename... Tail >
struct inherit_constructors: public Head, public inherit_constructors< Tail... > {
   using Head::Head;
   using inherit_constructors< Tail... >::inherit_constructors;
};

template < typename T >
struct inherit_constructors< T >: public T {
   using T::T;
};

/**
 * Cloneable Interface for when the base classes of EventT are also Cloneable
 * @tparam Derived,
 *      the actual class to clone
 * @tparam Bases,
 *      the cloneable base classes of EventT
 */
template < typename Derived, typename... Bases >
class Cloneable: public Bases... {
  public:
   virtual ~Cloneable() = default;

   uptr< Derived > clone() const
   {
      return uptr< Derived >(static_cast< Derived * >(this->clone_impl()));
   }

  private:
   Cloneable *clone_impl() const override
   {
      return new Derived(static_cast< const Derived & >(*this));
   }
};

template < typename Derived, typename... Bases >
class Cloneable< Derived, inherit_constructors< Bases... > >:
    public inherit_constructors< Bases... > {
  public:
   using inherit_constructors< Bases... >::inherit_constructors;
   virtual ~Cloneable() = default;

   uptr< Derived > clone() const
   {
      return uptr< Derived >(static_cast< Derived * >(this->clone_impl()));
   }

  private:
   Cloneable *clone_impl() const override
   {
      return new Derived(static_cast< const Derived & >(*this));
   }
};

template < typename Derived, typename... Bases >
class Cloneable< abstract_method< Derived >, Bases... >: public Bases... {
  public:
   virtual ~Cloneable() = default;

   uptr< Derived > clone() const
   {
      return uptr< Derived >(static_cast< Derived * >(this->clone_impl()));
   }

  private:
   virtual Cloneable *clone_impl() const = 0;
};

template < typename Derived, typename... Bases >
class Cloneable< abstract_method< Derived >, inherit_constructors< Bases... > >:
    public inherit_constructors< Bases... > {
  public:
   using inherit_constructors< Bases... >::inherit_constructors;
   virtual ~Cloneable() = default;

   uptr< Derived > clone() const
   {
      return uptr< Derived >(static_cast< Derived * >(this->clone_impl()));
   }

  private:
   virtual Cloneable *clone_impl() const = 0;
};

template < typename Derived >
class Cloneable< Derived > {
  public:
   virtual ~Cloneable() = default;

   uptr< Derived > clone() const
   {
      return uptr< Derived >(static_cast< Derived * >(this->clone_impl()));
   }

  private:
   virtual Cloneable *clone_impl() const
   {
      return new Derived(static_cast< const Derived & >(*this));
   }
};

template < typename Derived >
class Cloneable< abstract_method< Derived > > {
  public:
   virtual ~Cloneable() = default;

   uptr< Derived > clone() const
   {
      return uptr< Derived >(static_cast< Derived * >(this->clone_impl()));
   }

  private:
   virtual Cloneable *clone_impl() const = 0;
};

#endif  // LORAINE_TYPES_H
