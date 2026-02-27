#pragma once

// https://github.com/kotek900/STpointer

// usage: class MyClass : public STnode
// instead of: MyClass * object = new MyClass(arg1, arg2);
// use: STpointer<MyClass> object = STpointer<MyClass>(new MyClass(arg1, arg2))
// or use: STpointer<MyClass> object = STpointer<MyClass>(NEW, arg1, arg2)

// for fields of type STpointer inside objects inside their constructor: field.setParent(this)
// this - pointer to the parent object (extends STnode)

// see example usage in example.cpp on the github page

#ifndef DISABLE_NEW_IN_STPOINTER
#define ALLOW_NEW_IN_STPOINTER // delete this line if you don't want usage of STpointer<MyClass>(NEW, arg1, arg2)
#endif

#ifdef ALLOW_NEW_IN_STPOINTER
const bool NEW = true;
#endif

#include <vector>

template <typename T>
class STpointer;

class STnode {
private:
	std::vector<STpointer<STnode>*> referencesToThis;

	STnode(const STnode&) = delete;

	void addReferenceToThis(STpointer<STnode>* pointer) {
		this->referencesToThis.push_back(pointer);
	}

	void removeReferenceToThis(STpointer<STnode>* pointer) {
		for (int i = 0; i < this->referencesToThis.size(); i++) {
			if (this->referencesToThis[i] != pointer) continue;
			this->referencesToThis.erase(this->referencesToThis.begin() + i);
			return;
		}
	}

	template<typename U>
	friend class STpointer;
public:
	STnode() {
		referencesToThis = std::vector<STpointer<STnode>*>();
	};

	virtual ~STnode() {}
};

template <typename T>
class STpointer {
private:
	STnode * pointer;
	STnode * parent;

	bool isTree() {
		if (pointer == nullptr) return false;
		if ((void*)(pointer->referencesToThis[0]) == (void*)this) return true;
		else return false;
	}

	bool loopsTo(void * node) {
		if (parent == nullptr) return false;
		if (parent == node) return true;
		return parent->referencesToThis[0]->loopsTo(node);
	}

	// returns true if loop should be deleted
	bool unloop(void * stop) {
		if (parent == stop) return true;
		for (int i = 1; i < pointer->referencesToThis.size(); i++) {
			if (!pointer->referencesToThis[i]->loopsTo(stop)) {
				std::swap(pointer->referencesToThis[i], pointer->referencesToThis[0]);
				return false;
			}
		}
		return pointer->referencesToThis[0]->unloop(stop);
	}

	template<typename U>
	friend class STpointer;
public:
	STpointer() {
		pointer = nullptr;
		parent = nullptr;
	}

	#ifdef ALLOW_NEW_IN_STPOINTER
	template <typename... Args>
	STpointer(bool create, Args... args) {
		if (create == false) pointer = nullptr;
		else {
			pointer = new T(args...);
			pointer->addReferenceToThis((STpointer<STnode>*)this);
		}
		parent = nullptr;
	}
	#endif

	STpointer(STnode* value) {
		pointer = value;
		pointer->addReferenceToThis((STpointer<STnode>*)this);
		parent = nullptr;
	}

	STpointer(const STpointer& other) {
		this->pointer = other.pointer;
		this->pointer->addReferenceToThis((STpointer<STnode>*)this);
		this->parent = nullptr;
	}

	STpointer &operator=(const STpointer &other) {
		if (this == nullptr) return *this;
		if (&other==nullptr) {
			this->pointer = nullptr;
			if (this->pointer->referencesToThis.size() == 0) {
				delete pointer;
				return *this;
			}
			if (!parent->referencesToThis[0]->loopsTo(parent)) return *this;
			if (parent->referencesToThis[0]->unloop(parent)) delete pointer;
			return *this;
		}
		if (this->pointer == other.pointer) return *this;
		if (other.pointer == nullptr) {
			this->pointer->removeReferenceToThis((STpointer<STnode>*)this);
		} else if (this->pointer) {
			bool tree = this->isTree();
			this->pointer = other.pointer;
			this->pointer->removeReferenceToThis((STpointer<STnode>*)this);
			this->pointer->addReferenceToThis((STpointer<STnode>*)this);
			if (this->pointer->referencesToThis.size() == 0) {
				delete pointer;
				return *this;
			}
			if (!parent->referencesToThis[0]->loopsTo(parent)) return *this;
			if (parent->referencesToThis[0]->unloop(parent)) delete pointer;
		}
		return *this;
	}

	~STpointer() {
		if (this->pointer == nullptr) return;
		this->unset();
	}

	T& get() const {
		return *((T*)pointer);
	}

	T* operator->() const {
		return (T*)pointer;
	}

	T& operator*() const {
		return *((T*)pointer);
	}

	void setParent(STnode * parent) {
		this->parent = parent;
	}

	void unset() {
		if (this->pointer == nullptr) return;
		bool tree = this->isTree();
		this->pointer->removeReferenceToThis((STpointer<STnode>*)this);
		if (tree == false) goto reset;
		if (this->pointer->referencesToThis.size() == 0) {
			delete pointer;
			goto reset;
		}
		if (parent == nullptr) {
			if (pointer->referencesToThis[0]->loopsTo(pointer)) goto reset;
			if (pointer->referencesToThis[0]->unloop(pointer)) delete pointer;
			goto reset;
		}
		if (!parent->referencesToThis[0]->loopsTo(parent)) goto reset;
		if (parent->referencesToThis[0]->unloop(parent)) delete pointer;
		reset:
		this->pointer = nullptr;
	}
};
