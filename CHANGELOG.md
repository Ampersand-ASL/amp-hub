# Amp Hub Change Log

## 2026-01-30

- Per Frank (KG9M), not announcing connect/disconnect events to non-initiating
connections. This is particularly important to prevent conference participants
(or worse, repeater users) from being bothered by announcements. Only the call
that initiated the connect/disconnect operation will receive the announcement.
- Changed shape of low-pass filters used for decimation/interpolation
to improve audio quality. See https://mackinnon.info/ampersand/#improvements-on-app_rpt-resampling-filter


