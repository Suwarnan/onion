# Usage example #
`$ cat weather-forecasts.vert`
```
<doc id="1" name="Weather forecasts for Czech Republic">
<p>
Weather
forecast
for
Prague:
Partially
cloudy
with
occasional
showers.
Temperatures
around
15
degrees
Celsius.
North
wind
between
20
and
25
km/h.
</p>
<p>
Weather
forecast
for
Brno:
Partially
cloudy
with
occasional
showers.
Temperatures
around
16
degrees
Celsius.
North
wind
between
20
and
30
km/h.
</p>
</doc>
```

`$ onion -smq weather-forecasts.vert`
```
<doc id="1" name="Weather forecasts for Czech Republic">
<p>
Weather
forecast
for
Prague:
Partially
cloudy
with
occasional
showers.
Temperatures
around
15
degrees
Celsius.
North
wind
between
20
and
25
km/h.
</p>
</doc>
```

The second paragraph got removed since it is a near-duplicate of the first paragraph (according to the default settings).