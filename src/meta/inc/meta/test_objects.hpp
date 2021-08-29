#ifndef NIL_SRC_META_INC_META_TESTOBJECTS_HPP_
#define NIL_SRC_META_INC_META_TESTOBJECTS_HPP_

// TODO (SwappyG) - make utils folder and move this to there

namespace nil::test {

struct move_bench {
  move_bench() = default;
  move_bench(const move_bench& other);
  move_bench& operator=(const move_bench& other);
  move_bench(move_bench&& other) noexcept;
  move_bench& operator=(move_bench&& other) noexcept;
  bool IsMoved() { return moved_from_; }

 private:
  void AssertNotMoved_(const move_bench& other);

  bool moved_from_{false};
};

}  // namespace nil::test

#endif  // NIL_SRC_META_INC_META_TESTOBJECTS_HPP_