/*
** mrb_tinymt - tinymt class for mruby
**
** Copyright (c) mod_mruby developers 2012-
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "tinymt32.h"

#include <time.h>

#define DONE mrb_gc_arena_restore(mrb, 0);
#define RAND_SEED_KEY "$mrb_ext_rand_seed"

static void tinymt32_t_free(mrb_state *mrb, void *p)
{
}

static const struct mrb_data_type tinymt32_t_type = {
  "tinymt32_t", tinymt32_t_free,
};
 
static void tmt_srand(tinymt32_t *tmt, unsigned long seed)
{
  tinymt32_init(tmt, seed);
}  

static unsigned long tmt_rand(tinymt32_t *tmt)
{
  return tinymt32_generate_uint32(tmt);
}  

static double tmt_rand_real(tinymt32_t *tmt)
{
  return tinymt32_generate_float(tmt);
}  

static tinymt32_t *mrb_tinymt_get_context(mrb_state *mrb,  mrb_value self)
{
  tinymt32_t *tmt;
  mrb_value ctx;

  ctx = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "tinymt32_ctx"));
  Data_Get_Struct(mrb, ctx, &tinymt32_t_type, tmt);
  if (!tmt)
    mrb_raise(mrb, E_RUNTIME_ERROR, "redisContext get from mrb_iv_get redis_c failed");

  return tmt;
}

static mrb_value mrb_tinymt_init(mrb_state *mrb, mrb_value self)
{
  tinymt32_t *tinymt = (tinymt32_t *)mrb_malloc(mrb, sizeof(tinymt32_t));

  mrb_get_args(mrb, "fff", &tinymt->mat1, &tinymt->mat2, &tinymt->tmat);
  mrb_iv_set(mrb
    , self
    , mrb_intern_lit(mrb, "tinymt32_ctx")
    , mrb_obj_value(Data_Wrap_Struct(mrb
      , mrb->object_class
      , &tinymt32_t_type
      , (void*) tinymt)
    )
  );

  return self;
}

static mrb_value mrb_call_tinymt_srand(mrb_state *mrb, tinymt32_t *tinymt, mrb_value seed)
{ 
  if (mrb_nil_p(seed)) {
    seed = mrb_fixnum_value(time(NULL) + tmt_rand(tinymt));
    if (mrb_fixnum(seed) < 0) {
      seed = mrb_fixnum_value( 0 - mrb_fixnum(seed));
    }
  }

  tmt_srand(tinymt, (unsigned) mrb_fixnum(seed));

  return seed;
}

static mrb_value mrb_call_tinymt_rand(mrb_state *mrb, tinymt32_t *tinymt, mrb_value max)
{ 
  mrb_value value;

  if (mrb_fixnum(max) == 0) {
    value = mrb_float_value(mrb, tmt_rand_real(tinymt));
  } else {
    value = mrb_fixnum_value(tmt_rand(tinymt) % mrb_fixnum(max));
  }

  return value;
}

static mrb_value mrb_tinymt_rand(mrb_state *mrb, mrb_value self)
{
  mrb_value *argv;
  mrb_int argc;
  mrb_value max;
  tinymt32_t *tinymt = mrb_tinymt_get_context(mrb, self);

  mrb_get_args(mrb, "*", &argv, &argc);

  if (argc == 0) {
    max = mrb_fixnum_value(0);
  } else if (argc == 1) {
    max = argv[0];
    if (!mrb_nil_p(max) && !mrb_fixnum_p(max)) {
      max = mrb_check_convert_type(mrb, max, MRB_TT_FIXNUM, "Fixnum", "to_int");
    }
    if (!mrb_nil_p(max) && mrb_fixnum(max) < 0) {
      max = mrb_fixnum_value(0 - mrb_fixnum(max));
    }

    if (!mrb_fixnum_p(max)) {
      mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument type");
      return mrb_nil_value();
    }
  } else {
    mrb_raisef(mrb, E_ARGUMENT_ERROR, "wrong number of arguments (%d for 0..1)", argc);
    return mrb_nil_value();
  }

  mrb_value seed = mrb_gv_get(mrb, mrb_intern_lit(mrb, RAND_SEED_KEY));
  if (mrb_nil_p(seed))
    mrb_call_tinymt_srand(mrb, tinymt, mrb_nil_value());

  return mrb_call_tinymt_rand(mrb, tinymt, max);
}

static mrb_value mrb_tinymt_srand(mrb_state *mrb, mrb_value self)
{
  mrb_int argc;
  mrb_value *argv;
  mrb_value seed;
  tinymt32_t *tinymt = mrb_tinymt_get_context(mrb, self);

  mrb_get_args(mrb, "*", &argv, &argc);

  if (argc == 0) {
    seed = mrb_nil_value();
  } else if (argc == 1) {
    seed = argv[0];
    if (!mrb_nil_p(seed) && !mrb_fixnum_p(seed)) {
      seed = mrb_check_convert_type(mrb, seed, MRB_TT_FIXNUM, "Fixnum", "to_int");
    }
     if (!mrb_nil_p(seed) && mrb_fixnum(seed) < 0) {
      seed = mrb_fixnum_value(0 - mrb_fixnum(seed));
    }

    if (!mrb_fixnum_p(seed)) {
      mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument type");
      return mrb_nil_value();
    }
  } else {
    mrb_raisef(mrb, E_ARGUMENT_ERROR, "wrong number of arguments (%d for 0..1)", argc);
    return mrb_nil_value();
  }

  seed = mrb_call_tinymt_srand(mrb, tinymt, seed);
  mrb_value old_seed = mrb_gv_get(mrb, mrb_intern_lit(mrb, RAND_SEED_KEY));
  mrb_gv_set(mrb, mrb_intern_lit(mrb, RAND_SEED_KEY), seed);

  mrb_iv_set(mrb
    , self
    , mrb_intern_lit(mrb, "tinymt32_ctx")
    , mrb_obj_value(Data_Wrap_Struct(mrb
      , mrb->object_class
      , &tinymt32_t_type
      , (void*) tinymt)
    )
  );

  return old_seed;
}

void mrb_mruby_tinymt_gem_init(mrb_state *mrb)
{
  struct RClass *tmt;

  tmt = mrb_define_class(mrb, "TinyMT", mrb->object_class);

  mrb_define_method(mrb, tmt, "initialize", mrb_tinymt_init,  ARGS_ANY());
  mrb_define_method(mrb, tmt, "rand",       mrb_tinymt_rand,  ARGS_ANY());
  mrb_define_method(mrb, tmt, "srand",      mrb_tinymt_srand, ARGS_ANY());
  DONE;
}

void mrb_mruby_tinymt_gem_final(mrb_state *mrb)
{
}

