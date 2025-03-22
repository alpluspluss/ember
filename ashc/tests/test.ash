/* test file for ash */

function add(a: i32, b: i32) -> i32 
{
    return a + b;
}

struct Point 
{
    var x: f32 = 0.0;
    var y: f32 = 0.0;
};

const PI: f64 = 3.14159265359;

var numbers: i32[] = { 1, 2, 3, 4, 5 };

function test_operators() -> void 
{
    var a = 10;
    var b = 20;
    
    var c = a + b;      // add
    c -= 5;             // sub
    c *= 2;             // mul
    c /= 5;             // div
    c %= 3;             // mod
    
    var d = a & b;      // band
    var e = a | b;      // bor
    var f = a ^ b;      // bxor
    var g = ~a;         // bnot
    
    var h = a << 2;     // bshl
    var i = b >> 1;     // bshr
    
    var j = a == b;     // eq
    var k = a != b;     // neq
    var l = a < b;      // lt
    var m = a > b;      // gt
    var n = a <= b;     // lteq
    var o = a >= b;     // gteq
    
    var p = a && b;     // land
    var q = a || b;     // lor
    var r = !a;         // lnot
}

/* literal */
var message: string = "Hello, world!";
var hex_value: u32 = 0xABCD;
var binary_value: u8 = 0b1010;
var float_value: f32 = 3.14132;