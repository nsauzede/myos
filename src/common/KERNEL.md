
kb_handler =>
- pushes to kb_ring/kb_head/panic

tkb =>
- move kb_tail/kb_head to kcodes/ovf
- on newline, flush to stdin/stdin_head/panic

tread =>
- wait for lock, flush stdin to buf
