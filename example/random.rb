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
