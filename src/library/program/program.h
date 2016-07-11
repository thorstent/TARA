/*
 * Copyright 2014, IST Austria
 *
 * This file is part of TARA.
 *
 * TARA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TARA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TARA.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TARA_PROGRAM_H
#define TARA_PROGRAM_H

#include "helpers/helpers.h"
#include "helpers/z3interf.h"
#include "api/options.h"
#include "hb_enc/symbolic_event.h"
// #include "cinput/cinput_exception.h"

namespace tara {
  class loc{
  public:
    unsigned line;
    unsigned col;
    std::string file;
  };

  class thread {
  public:
    thread(helpers::z3interf& z3_, std::string name_):
      z3(z3_), name(name_) {}
    hb_enc::se_ptr start_event, final_event;
    helpers::z3interf& z3;
    std::string name;
    hb_enc::se_vec events; // topologically sorted events
    z3::expr phi_ssa = z3.mk_true();
    z3::expr phi_prp = z3.mk_false();
    void add_event( hb_enc::se_ptr e ) { events.push_back( e ); }
    void set_start_event( hb_enc::se_ptr e ) { start_event = e; }
    void set_final_event( hb_enc::se_ptr e ) { final_event = e; }
    void append_ssa( z3::expr e) {
      phi_ssa = phi_ssa && e;
    }

    void append_property( z3::expr prp) {
      phi_prp = phi_prp || prp;
    }
  };

  class program {
  private:
    unsigned thread_num = 0;
    std::vector<thread> threads;
    mm_t mm = mm_t::none;

  public:
    program(helpers::z3interf& z3_): z3(z3_) { add_thread("caller"); }
    helpers::z3interf& z3;
    cssa::variable_set globals;
    cssa::variable_set allocated; // temp allocations
    std::map< unsigned, loc> inst_to_loc;
    std::map< std::string, hb_enc::se_ptr> create_map, join_map;
    hb_enc::name_to_ses_map se_store;


    bool is_mm_declared() const;
    bool is_wmm() const;
    bool is_mm_sc() const;
    bool is_mm_tso() const;
    bool is_mm_pso() const;
    bool is_mm_rmo() const;
    bool is_mm_alpha() const;
    bool is_mm_power() const;
    void set_mm( mm_t );
    mm_t get_mm() const;
    void unsupported_mm() const;

    inline unsigned no_of_threads() const {
      return thread_num;
    }

    inline const thread& get_thread(unsigned t) const {
      return threads[t];
    }

    unsigned add_thread( std::string str) {
      thread thr( z3, str );
      threads.push_back( thr );
      thread_num++;
      return thread_num-1;
    }

    void append_ssa( unsigned thread_id, z3::expr e) {
      threads[thread_id].append_ssa( e );
    }

    void append_property( unsigned thread_id, z3::expr prp) {
      threads[thread_id].append_property( prp );
    }


    void add_event( unsigned i, hb_enc::se_ptr e ) {
      threads[i].add_event( e );
      se_store[e->name()] = e;
    }

    void add_event( unsigned i, hb_enc::se_set es ) {
      for( auto e : es ) {
        add_event( i, e );
      }
    }

    void set_start_event( unsigned i, hb_enc::se_ptr e ) {
      threads[i].set_start_event( e );
    }

    void set_final_event( unsigned i, hb_enc::se_ptr e ) {
      threads[i].set_final_event( e );
    }

    void add_create( unsigned thr_id, hb_enc::se_ptr e, std::string fname ) {
      add_event( thr_id, e );
      if( create_map.find( fname) != create_map.end() )
        program_error( "function launch multiple times not supported!" );
      create_map[ fname ] = e;
    }

    void add_join( unsigned thr_id, hb_enc::se_ptr e, std::string fname ) {
      add_event( thr_id, e );
      if( join_map.find( fname) != create_map.end() )
        program_error( "function launch multiple times not supported!" );
      join_map[ fname ] = e;
    }

    void add_global( std::string g, z3::sort sort ) {
      globals.insert( cssa::variable(g, sort) );
    }

    cssa::variable get_global( std::string gname ) {
      for( auto& g : globals ) {
        if( gname == g.name )
          return g;
      }
      program_error( "global variable " << gname << " not found!" );
      cssa::variable g(z3.c); // dummy code to suppress warning
      return g;
    }

    void add_allocated( std::string g, z3::sort sort ) {
      allocated.insert( cssa::variable(g, sort) );
    }

    cssa::variable get_allocated( std::string gname ) {
      for( auto& g : allocated ) {
        if( gname == g.name )
          return g;
      }
      program_error( "allocated name " << gname << " not found!" );
      cssa::variable g(z3.c); // dummy code to suppress warning
      return g;
    }

  };

inline bool program::is_mm_declared() const {  return mm != mm_t::none; }
inline bool program::is_wmm()         const {  return mm != mm_t::sc;   }
inline bool program::is_mm_sc()       const {  return mm == mm_t::sc;   }
inline bool program::is_mm_tso()      const {  return mm == mm_t::tso;  }
inline bool program::is_mm_pso()      const {  return mm == mm_t::pso;  }
inline bool program::is_mm_rmo()      const {  return mm == mm_t::rmo;  }
inline bool program::is_mm_alpha()    const {  return mm == mm_t::alpha;}
inline bool program::is_mm_power()    const {  return mm == mm_t::power;}

inline mm_t program::get_mm()         const { return mm; }
inline void program::set_mm(mm_t _mm)       { mm = _mm;  }

inline void program::unsupported_mm() const {
  std::string msg = "unsupported memory model: " + string_of_mm( mm ) + "!!";
  program_error( msg );
}


}
#endif // TARA_PROGRAM_H