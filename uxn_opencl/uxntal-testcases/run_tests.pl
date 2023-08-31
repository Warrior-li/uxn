#!/usr/bin/perl
use strict;
use warnings;
use v5.30;
use Getopt::Std;
my %opts = ();
getopts( 'hvruDt:f:', \%opts );

if ($opts{'h'}){
    die "
    $0 -[hvut] 

    -v: verbose
    -u: test Uxntal emitter, not C emitter
    -r: registerised or not
    -t: test mode. Either 'passing', 'failing' or a file name for a single test. Defaults to 'passing'
    -f: from. Runs tests from the given name
    \n";
}
my $test_mode = $opts{'t'} 
    ?
        $opts{'t'} =~/^p/ ? 'passing' : 
        $opts{'t'} =~/^f/ ? 'failing' : 'single'
    : 'passing';
my $single_test = $test_mode eq 'single' ? $opts{'t'} : '';

my @refs_pass=( 
'',
'*',
'*',
'*',
'*',
'*',
'*',
'*',
'*',
'*',
'&',
'&',
'&',
'l',
'Hello, World!',
'Hello, World!',
'h',
'4',
'Hello, World!',
'Hello!',
'*',
'H',
'u',
'#',
'#',
'=',
'=',
'',
'F',
'd',
'4',
'6',
'6',
'F',
'h',
'*',
''
);


my @refs_fail = (
'*',
'H',
'D',
'123456789:;<=>?@',
'00001
00002
00003
00004
00005
00006
00007
00008
00009
00010
00011
00012
00013
00014
00015
00016',
'*',
'&',
'*',
'00001
00002
00003
00004
00005
00006
00007
00008
00009
00010
00011
00012
00013
00014
00015
00016',
'e',
'6',
'6',
'F',
'H',
# new tests

'C',
'H',
'F',
'F',
'*',
'F',
'Q',
'6',
'6',
'*',
'*',
'**',
'*'


);

my @passing_tests = qw(
    ex00_basics.tal
    ex01_0_simple_calc.tal
    ex01_1_simple_calc.tal
    ex01_2_simple_calc.tal
    ex01_3_simple_calc.tal
    ex01_simple_calc.tal
    ex02_simple_calc_8bit.tal
    ex03_simple_calc_registers.tal
    ex04_simple_calc_registers_8bit.tal
    ex05_simple_calc_registers2.tal
    ex06_1_subroutine-call.tal
    ex06_subroutine-call.tal
    ex07_subroutine-call_8bit.tal

    ex08_2_function_pointer.tal

    ex10_hello-world.tal
    ex10_1_hello_world_reg.tal
    ex10_2_hello-world_global_reg.tal
    ex10_2_hello_world_reg.tal
    ex10_3_hello_world_reg_ok.tal
    ex10_5_hello-world_func_no_loop.tal

    ex11_3_phi.tal
    ex11_6_rel_labels.tal


    ex13_functions.tal
    ex13_1_functions.tal
    ex13_2_functions.tal

    ex14_functions.tal
    ex14_1_functions.tal
    ex14_2_functions.tal
    ex14_3_functions.tal
    ex14_4_functions.tal
    ex14_5_functions.tal

    ex15_functions.tal

    ex16_functions.tal

    ex17_function_mult_ret.tal

    ex18_stack_mixed_sizes.tal
    ex19_nested_function_8bit.tal

);

my @failing_tests = qw(

    ex08_memory.tal
    ex08_1_function_pointer.tal        
    ex09_memory_8bit.tal

    ex11_1_phi.tal
    ex11_2_loop_registered.tal
    ex11_2_phi.tal

    ex11_4_phi.tal
    ex11_5_phi.tal

    ex11_phi.tal

    ex12_functions.tal

    ex15_functions_registerised.tal

    ex16_functions_registered.tal

    ex17_function_mult_ret_registered.tal
    
    
    ex20_1_rel_abs_labels.tal

    ex20_1_registerised.tal
    ex20_1_rel_abs_labels.tal
    ex20_2_rel_abs_labels_man.tal
    ex20_2_rel_abs_labels.tal
    ex20_3_no_labels.tal
    ex20_4_no_JCN.tal
    ex20_5_args_retvals_bug.tal
    ex21_1_function_types.tal
    ex21_function_types.tal
    ex22_1_JCN_args.tal
    ex22_2_JMP_args.tal
    ex22_3_order_of_args.tal
    ex23_return_stack.tal

    );

    

my %ref_for_test = ();
my $i=0;
if ($test_mode eq 'passing')  {
    for my $test (@passing_tests) {
        my $ref = $refs_pass[$i++];
        $ref_for_test{$test}=$ref;
    }
} elsif ($test_mode eq 'failing') {
    for my $test (@failing_tests) {
        my $ref = $refs_fail[$i++];
        $ref_for_test{$test}=$ref;
    }
} else {
  for my $test (@passing_tests) {
        my $ref = $refs_pass[$i++];
        $ref_for_test{$test}=$ref;
    }
    $i=0;
    for my $test (@failing_tests) {
        my $ref = $refs_fail[$i++];
        $ref_for_test{$test}=$ref;
    }    
}

my $from = $opts{'f'} 
    ? $opts{'f'} 
    : $test_mode eq 'passing' 
        ? $passing_tests[0] 
        : $failing_tests[0];

my @tests = ($test_mode eq 'passing') 
    ? @passing_tests 
    : ($test_mode eq 'failing')
        ? @failing_tests 
        : ($single_test);

my @refs = ($test_mode eq 'passing') 
    ? @refs_pass 
    : ($test_mode eq 'failing')
        ? @refs_fail 
        : ($ref_for_test{$single_test});

my $n_tests=scalar @tests; 
my $test_ct=1;
my $skip=$single_test?0:1;

for my $test (@tests) { 
    my $ref = shift @refs;
    $skip = $test eq $from ? 0 : $skip;
    next if $skip;
    say "$test_ct/$n_tests:\t$test:"; 
    system("cp $test tmp.tal");
    system("../bin/uxnasm tmp.tal tmp.rom >/dev/null 2>&1");
    system("(echo -n q | ../bin/uxncli tmp.rom) > output.txt &");

    # Wait for a short time to allow uxncli to finish
    sleep(1);

    # Read the content of the output file
    my $res = do {
        local $/ = undef;
        open my $fh, "<", "output.txt" or die $!;
        <$fh>;
    };

    say $res;
    chomp $res;

    say "$res: ", ($res eq $ref ? 'PASS' : 'FAIL (<'.$res.'><><'.$ref.'>)' );

    say '----';
    $test_ct++;
}
