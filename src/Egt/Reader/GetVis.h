/*
 * GetVis.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_READER_GETVIS_H_
#define EGT_READER_GETVIS_H_

#include <type_traits>
#include "boost/variant/static_visitor.hpp"
#include <stdexcept>

namespace Egt
{
template<typename T>
struct GetVis : boost::static_visitor<T>
{
	template<typename U> 	T get(const U& u, std::true_type ) {return u;}
	template<typename U> 	T get(const U& u, std::false_type) {throw std::runtime_error("Error in GetVis"); return T();}

	template<typename U>
	T operator()(const U& u) {return get(u, typename std::is_same<T, U>::type());}


};
}



#endif /* EGT_READER_GETVIS_H_ */
