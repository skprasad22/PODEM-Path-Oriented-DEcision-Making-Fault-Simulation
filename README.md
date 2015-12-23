# PODEM-Path-Oriented-DEcision-Making-Fault-Simulation
DESCRIPTION

PODEM is a Test Generation algorithm characterized by a direct search process, in which decision consist of only PI (Primary Input) assignments. Unlike many other TG algorithms, PODEM does not have any line justification problems.

PODEM treats a value vk to be justified for line k as an OBJECTIVE (k, vk) to be achieved via PI assignments. A backtracking procedure maps a desired objective into a PI assignment that is likely to contribute to achieving the objective. The PI assignment returned by Backtrace (k, vk) is (j, vj). An x-path from k to j is chosen. No values are assigned during backtracing. Values are assigned only by simulating PI assignments, done by Imply().

STEPS

1. Put the text file containing the circuit in the same folder as that of the code.
2. Enter the filename without .txt.
3. Proceed further based on the options you select.
