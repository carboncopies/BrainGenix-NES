# Simulating

Some notes on the processes that take place during a simulation run. These notes should help to clarify what features and capabilities need to be available in a chosen computatonal backend for NN simulations (e.g. NEST, custom code, etc.).

## List of neurons (LoN) vs Circuits with neurons (CwN)

The simulator currently allows two ways to describe a model and to run it:

1. A List of neurons: This is simply a (potentially large) list of neurons with all of their attributes. Everything is considered as a single network and simulaton processing is done by running through the list of neurons.

2. A set of neural circuits, each of which contains a list of neurons: This creates a logical description of the whole "brain" as a series of regions, each of which contains a neuronal circuit. Simulations are run by going through the list of circuits and then within each of those the corresponding list of neurons.

## Iterations

At present, our simulations use a synchronous clock for updates of all components.

- The simulation interval is specified.
- The duration of the simulation is specified.

At each step the following is done:

1. Record the simulation time.

2. For each neuron (depending on LoN or CwN method), update the
   neuron and record states.

3. Carry out simulated experimental instrument recordings.

## Neuron Updates

Over the time-interval since the last time step:

1. Check if a specified direct stimulation occurred that
   elicit a spike at the neuron. If so, update the list of
   recorded spike times.

2. Update the neuron's state variables to reflect changes
   of membrane potential, etc. (See detailing below.)

3. Detect if the membrane potential exceeds the firing
   threshold. This takes into account whether the neuron
   is in its absolute refractory period. If firing takes
   place then update the list of recorded spike times.

4. Check if probabilistic spontaneous firing of the neuron
   occurs. This also checks if the neuron is in its
   absolute refractory period, and whether a probability
   distribution was defined for spontaneous firing. If
   spontaneous firing occurs then update the list of
   recorded spike times.


### Neuron State Variable Updates

Updating a neuron's state variables involves the following steps:

1. Update the time interval since the neuron's last spike.

2. Calculate the contribution of the most recent action
   potential to the current membrane potential.

3. Calculate the contribution of after-hyperpolarization to
   the current membrane potential.

4. Calculate the contribution of postsynaptic potentials
   to the current membrane potential. Postsynaptic potentials
   contributed by each of the incoming receptors are represented
   by double-exponential functions and the use the the spike
   time of the presynaptic neuron.

5. Calculate the complete, updated membrane potential.

6. Update a FIFO buffer with the scaled difference between
   resting potential and current potential, clipped to
   positive values only. This buffer is used for simulated
   fluorescence in virtual calcium imaging.

## Instrument Recording

This involves the following:

1. Record the instrument recording time.

2. Carry out virtual Electrode recordings for each Electrode.

3. Carry out virtual Calcium Imaging.

### Virtual Electrode Recording

At each Electrode:

1. Record the recording time.

2. For each recording site on the electrode, calculate the electric
   field potential and record that.

The electric field potential at a specific site is calculated by
combining the effects of the membrane potentials at all neurons,
scaled the effect of the distance from the corresponding neuron.
Noise is added as specified.

### Virtual Calcium Imaging

Calcium imaging typicallt takes place at a slower frame rate than the
update intervals used in the neural simulation. When another Ca
Imaging frame is due, the following takes place for each of the
neurons that were specified to belong to the set of fluorescing
neurons:

1. Carry out a convolution of a 1-D fluorescence kernel with the
   contents of the FIFO buffer of relative membrane values of the
   neuron.

2. Record the shifted resulting value and the recording time.

The recorded values are later used to determine the fluorescence levels
of the 3D components of the neuron to generate a series of images based
on the portion of the neuronal networks structure that is visible in
the imaged regions (i.e. light sheet).

--
Randal A. Koene, 2024
