#ifndef EMC_BUFFER_H
#define EMC_BUFFER_H

#include <array>
#include <string>
#include <algorithm>
#include <cassert>
#include "exceptions.h"

namespace emc {
    template<unsigned short SIZE, typename streamType>
    class lookup_buffer {
        using ushort = unsigned short;
        using uchar = unsigned char;
        using buffer_type = std::array<uchar, SIZE>;
    public:        
        lookup_buffer(const std::string& lookup)
        :m_is_found(false), m_length_content(0), m_lookup_index(0), m_content_index(0) {
            if(lookup.empty()) {
                throw emc::exceptions::lookup_buffer::empty_string();
            }
            auto lookup_len = lookup.length();
            if(lookup_len > SIZE) {
                throw emc::exceptions::lookup_buffer::too_long_string();
            }
            std::copy(std::begin(lookup), std::end(lookup), std::begin(m_buff));
            m_length_lookup = lookup_len;
        }

        std::string get_lookup() const {
            return std::string(std::begin(m_buff), std::begin(m_buff) + m_length_lookup); 
        }

        std::string get_content() const {
            auto b = std::begin(m_buff) + m_length_lookup;
            auto e = b + m_length_content;
            return std::string(b, e);
        }

        bool search_in(streamType& stream, bool find_next_word) {
        //bool search_in(Concurrency::streams::streambuf<uint8_t> stream) {
            bool found = false;
            auto free_space = get_free_space_buffer();
            auto begin_content = get_content_begin(); 
            size_t read_len = 0;
            while(!found && !stream.is_eof()) {
                read_len = stream.getn(begin_content, free_space).get();
                if(read_len > 0) {
                    m_length_content = read_len;
                    found = search_in_buffer(free_space);
                }
            }
            m_is_found = found;
            if(m_is_found) {
                get_next_word(stream, true);
            }
            return found;
        }

        std::string get_next_word(streamType& stream, bool use_single_buffer) {
        //std::string get_next_word(Concurrency::streams::streambuf<uint8_t> stream) {
            if(!m_is_found) {
                throw emc::exceptions::lookup_buffer::not_found();
            }
            /* 
                HACK: That was the original plan, but MSVC++ doesn't like me today...
            */
            // get the index following the last character seen from the lookup
            size_t index = m_content_index;
            if(SIZE > index) 
            {
                ++index;
            }
            // how many characters do we have to keep
            //size_t distance = SIZE - index;
            // keep the last character from the content and place them at the front
            // of the buffer
            //std::copy_backward(m_buff[index], m_buff[index + distance], m_buff[0]);
            // and then return a string where starting from the start of the buffer
            // which would have been the end of the lookup word up to the word, but MSVC++ happened...
            
            // The new plan is too keep looking from the current position
            auto b = std::begin(m_buff) + index;
            auto e = std::end(m_buff);
            auto itt = std::find(b, e, ' ');
            std::string word(b, itt);
            if(e == itt) {
                // we haven't found a space yet
                size_t distance = SIZE - m_content_index;
                size_t read_len = stream.getn(&m_buff[0], distance).get();
                if(read_len > 0)
                {
                    b = std::begin(m_buff);
                    e = b + distance;
                    itt = std::find(b, e, ' ');
                    word.append(b, e == itt ? e : itt);
                }
                // else... no more data... so that's it.
            }
            assert(word.length() <= SIZE);
            if(use_single_buffer) {
                std::memset(&m_buff[0], 0, SIZE);
                std::copy(std::begin(word), std::end(word), std::begin(m_buff));
                m_length_lookup = 0;
                m_length_content = word.length();
                m_lookup_index = 0;
                m_content_index = 0;
            }
            return word;
        }

        bool is_lookup_found() const {
            return m_is_found;
        }
    private:
       uchar* const get_content_begin() {
            return &m_buff[m_length_lookup];
        }
        ushort get_free_space_buffer() const {
            return SIZE - m_length_lookup;
        }

        bool search_in_buffer(size_t content_length) {
            for(unsigned int i = m_length_lookup; !m_is_found && i < content_length + m_length_lookup; ++i) {
                if(m_buff[m_lookup_index] == m_buff[i]) {
                    ++m_lookup_index;
                    m_is_found = m_length_lookup == m_lookup_index;
                    if(m_is_found) {
                        m_content_index = i;
                    }
                } else {
                    m_lookup_index = 0;
                }
            }
            return m_is_found;
        }

        ushort m_length_lookup;
        ushort m_length_content;
        ushort m_lookup_index;
        ushort m_content_index;
        bool m_is_found = false;
        buffer_type m_buff;
    };
}

#endif
