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
		if (pointer->referencesToThis.size() == 0) return false;
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
		if (parent == nullptr) return false;
		for (int i = 1; i < pointer->referencesToThis.size(); i++) {
			if (!pointer->referencesToThis[i]->loopsTo(stop)) {
				std::swap(pointer->referencesToThis[i], pointer->referencesToThis[0]);
				return false;
			}
		}
		return parent->referencesToThis[0]->unloop(stop);
	}

	void setPointer(STnode * pointer) {
		if (pointer) pointer->addReferenceToThis((STpointer<STnode>*)this);
		if (this->pointer) this->unset();
		this->pointer = pointer;
	}

	void deleteObject() {
		STnode * toDelete = pointer;
		pointer->referencesToThis.clear();
		this->pointer = nullptr;
		delete toDelete;
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
			this->pointer = nullptr;
			setPointer(new T(args...));
		}
		parent = nullptr;
	}
	#endif

	STpointer(STnode* value) {
		this->pointer = nullptr;
		setPointer(value);
		parent = nullptr;
	}

	STpointer(const STpointer& other) {
		this->pointer = nullptr;
		setPointer(other->pointer);
		this->parent = nullptr;
	}

	STpointer &operator=(const STpointer &other) {
		if (this == nullptr) return *this;
		if (&other==nullptr) {
			unset();
			return *this;
		}
		setPointer(other.pointer);
		return *this;
	}

	~STpointer() {
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
			deleteObject();
			return;
		}
		if (!pointer->referencesToThis[0]->loopsTo(pointer)) goto reset;
		if (pointer->referencesToThis[0]->unloop(pointer)) {
			deleteObject();
		}
		return;
		reset:
		this->pointer = nullptr;
	}
};
