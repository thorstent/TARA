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
 *
 */

#ifndef TARA_PRUNE_UNSAT_CORE_H
#define TARA_PRUNE_UNSAT_CORE_H

#include "prune/prune_base.h"
#include "hb_enc/cycles.h"

namespace tara {
namespace prune {
class unsat_core : public prune::prune_base
{
public:
  unsat_core(const helpers::z3interf& z3, const tara::program& program, z3::solver sol_good);
  virtual std::list< z3::expr > prune(const std::list< z3::expr >& hbs, const z3::model& m);
  //todo : remove the code above
  virtual hb_enc::hb_vec prune( const hb_enc::hb_vec& hbs, const z3::model& m );
  virtual std::string name();
private:
  z3::solver sol_good;
  hb_enc::cycles rf_scc_finder;
  hb_enc::hb_vec find_thin_cycles( const hb_enc::hb_vec& hbs );
  bool find_thin_air_cycles( const hb_enc::hb_vec& hbs,
                             const z3::model& m,
                             hb_enc::hb_vec& involved_rfs );
private:
  void dump_sat_exec( std::list< z3::expr >& hbs_expr, std::string name="" );

};
}}

#endif // PRUNE_UNSAT_CORE_H
