---
title: Overview
brief: What is <strong>Aten</strong>, and what can it do
taxonomy:
  category: docs
template: manpage
docroot: /aten/docs
header_class: alt
---

**Aten** will let you generate and edit coordinates for your simulations, and view any trajectories you might have generated. A set of tools in the GUI (and also accessible from the command line) enables you to change the geometry of bonds, angles, and torsions, translate atoms, create atoms, rotate groups of atoms, and cut, copy, and paste them around the place. All this can be done in the context of loading and saving in the format that you need - if the file format you need isn't currently a part of **Aten**, you yourself can write a filter to cover it.

Periodic systems are supported, be they crystals, liquids, gases, or a heady mixture. All editing functions that are possible for simple molecules apply to periodic systems as well. Moreover, given a basic unit cell a whole crystal or a larger supercell can be constructed. For any periodic system, a random ensemble of molecules can be added, allowing the facile creation of coordinates with which to begin your molecular dynamics simulations.

As well as coordinates, **Aten** has support for forcefields (in its own, plain-text format) and can automatically apply these forcefields to your system if correct type descriptions are present for the atom types in the forcefield. Then, in the same way as with coordinates, you may write out a forcefield description of your system in the format that you require it with a different filter. Please don't use **Aten** as a literal ‘black box’, though, and blindly write out forcefield files without checking them. While it will certainly make the process of generating your forcefield descriptions easier, the art of determining the correct types in the first place (and hence the correct forcefield terms) is not definite for larger forcefields that cover many atom types. Check the output – a cursory glance of the selected forcefield types is an excellent idea, and a good habit to get in to.

**Aten** is in continual development, so if you get stuck, find a bug, or have a suggestion to make, please go to the Support page of the website and visit the forums or send an email directly. Making **Aten** better depends to some degree on user feedback!

