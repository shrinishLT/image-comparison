require '_h2ph_pre.ph';

no warnings qw(redefine misc);

unless(defined(&__sigval_t_defined)) {
    eval 'sub __sigval_t_defined () {1;}' unless defined(&__sigval_t_defined);
}
1;
