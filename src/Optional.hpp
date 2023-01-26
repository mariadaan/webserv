#ifndef OPTIONAL_HPP
 #define OPTIONAL_HPP

// https://en.cppreference.com/w/cpp/utility/optional

/**
 * Usage: Optional<SomeClass> opt;
 * Usage: Optional<SomeClass> opt(instance_of_some_class);
 *
 * SomeClass must have a default constructor. That is public or protected.
 */

template <typename T>
class Optional : public T
{
public:
	Optional() : _is_set(false) {}
	Optional(T t) : T(t), _is_set(true) {}
	bool is_set(void) const { return _is_set; }

private:
	bool _is_set;
};

#endif
