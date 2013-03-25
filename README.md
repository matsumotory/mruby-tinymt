# rand and srand method with TinyMT 
rand and srand method with [Tiny Mersenne Twister](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/TINYMT/index.html)

## install by mrbgems
 - add conf.gem line to `build_config.rb`
```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :git => 'https://github.com/matsumoto-r/mruby-tinymt.git'
end
```

## example

```ruby

# generate parameter from tinymt32dc
tmt = TinyMT.new(0xf50a1d49, 0xffa8ffeb, 0x0bf2bfff)

tmt.srand(5)
p tmt.rand()
tmt.srand(5)
p tmt.rand()
tmt.srand(10)
p tmt.rand()

5.times do |i|
  p "---" + i.to_s + "---"
  tmt.srand
  p tmt.rand
  p tmt.rand(0)
  p tmt.rand(25)
  p tmt.rand(50)
  p tmt.rand(75)
  p tmt.rand(100)
end
```

# License
under the MIT License:

* http://www.opensource.org/licenses/mit-license.php

tinymt32.{c.h} under the BSD New License:

* http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/TINYMT/LICENSE.txt
