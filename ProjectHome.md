## A tool for the analysis of two-dimensional point sets with applications in graphics ##

<font color="#FF0000'">April 2013: new version 1.1</font>

_psa_ is a command line tool that takes two-dimensional point sets as input
and analyzes these point sets with respect to several spatial and spectral
statistics important for sampling problems in computer graphics. These
statistics include:

- Global minimum distance (smallest separation between any two points)<br>
- Average minimum distance (average nearest-neighbour distance)<br>
- Bond-orientational order (similarity to the hexagonal lattice)<br>
- Effective Nyquist frequency (equivalent to Nyq. freq. in uniform sampling)<br>
- Oscillations (metric for potential aliasing caused by the point set)<br>

- Radial power spectrum<br>
- Radial distribution function<br>
- Anisotropy<br>
- Fourier amplitude/power spectrum<br>

For more information regarding these measures, consult our papers:<br>
<br>
Daniel Heck, Thomas Schlömer, Oliver Deussen<br>
<b>Blue Noise Sampling with Controlled Aliasing</b><br>
ACM Transactions on Graphics, 32(3):25:1-12, 2013<br>
<a href='http://graphics.uni-konstanz.de/publikationen/2013/controlled_aliasing/website/'>Paper Website</a>

Thomas Schlömer, Oliver Deussen<br>
<b>Accurate Spectral Analysis of Two-Dimensional Point Sets</b><br>
Journal of Graphics, GPU, and Game Tools, 15(3):152-160, 2011<br>
<a href='http://dx.doi.org/10.1080/2151237X.2011.609773'>Paper</a>

<i>psa</i> provides the exact same algorithms as used for the analysis of point sets in the papers:<br>
<br>
Thomas Schlömer, Daniel Heck, Oliver Deussen<br>
<b>Farthest-Point Optimized Point Sets with Maximized Minimum Distance</b><br>
HPG '11: High Performance Graphics Proceedings, 135-154, 2011<br>
<a href='http://cms.uni-konstanz.de/informatik/deussen/publikationen/'>Website</a>

Michael Balzer, Thomas Schlömer, Oliver Deussen:<br>
<b>Capacity-Constrained Point Distributions: A Variant of Lloyd's Method</b><br>
ACM Transactions on Graphics (Proceedings of SIGGRAPH), 28(3):86:1-8, 2009<br>
<a href='http://graphics.uni-konstanz.de/publikationen/2009/capacityconstrainedpointdistributions/website/'>Paper Website</a> - <a href='http://ccvt.googlecode.com'>Project</a>

Any suggestions regarding concept or implementation of <i>psa</i> are appreciated.