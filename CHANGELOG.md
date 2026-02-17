# Amp Hub Change Log

## Next

- Upgrade to the latest core that has some audio speed improvements.
- Added support for the G.726 CODEC.

## 2026-02-04

- More work cleaning up excessive announcements.
- An announcement has been added when a connection fails. This will 
only be directed th the originator of the DTMF connection request
and anyone who has entered DTMF commands within the last 30 seconds.
- Talker ID feature has been added to pass call sign/name through
the network. 
- Updated the coefficients on the various low-pass filters to improve
audio performance.
- DTMF *76 will put the hub channel into parrot mode (not audible to 
any other connections)

## 2026-01-30

- Per Frank (KG9M), not announcing connect/disconnect events to non-initiating
connections. This is particularly important to prevent conference participants
(or worse, repeater users) from being bothered by announcements. Only the call
that initiated the connect/disconnect operation will receive the announcement.
- Changed shape of low-pass filters used for decimation/interpolation
to improve audio quality. See https://mackinnon.info/ampersand/#improvements-on-app_rpt-resampling-filter
- Improved logging around registration errors



