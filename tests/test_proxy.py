# -*- coding: utf-8 -*-
# Pitivi video editor
# Copyright (c) 2019, Yatin Maan <yatinmaan1@gmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, write to the
# Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
# Boston, MA 02110-1301, USA.
"""Tests for the utils.proxy module."""
# pylint: disable=protected-access,too-many-arguments
from unittest import mock

from gi.repository import GES

from tests import common


class TestProxyManager(common.TestCase):
    """Tests for the ProxyManager class."""

    def _check_scale_asset_resolution(self, asset_res, max_res, expected_res):
        app = common.create_pitivi_mock()
        manager = app.proxy_manager

        stream = mock.Mock()
        stream.get_width.return_value = asset_res[0]
        stream.get_height.return_value = asset_res[1]

        asset = mock.Mock()
        asset.get_info().get_video_streams.return_value = [stream]

        result = manager._scale_asset_resolution(asset, max_res[0], max_res[1])
        self.assertEqual(result, expected_res)

    def test_scale_asset_resolution(self):
        """Checks the _scale_asset_resolution method."""
        self._check_scale_asset_resolution((1920, 1080), (100, 100), (96, 54))
        self._check_scale_asset_resolution((1080, 1920), (100, 100), (54, 96))
        self._check_scale_asset_resolution((1000, 1000), (100, 100), (100, 100))

        # Unscalable resolutions.
        self._check_scale_asset_resolution((1000, 10), (100, 100), (1000, 10))
        self._check_scale_asset_resolution((10, 1000), (100, 100), (10, 1000))
        self._check_scale_asset_resolution((100, 100), (200, 200), (100, 100))

    def _check_getTargetUri(self, proxy_uri, expected_uri):
        app = common.create_pitivi_mock()
        manager = app.proxy_manager

        asset = mock.Mock(spec=GES.Asset)
        asset.props.id = proxy_uri

        result = manager.getTargetUri(asset)
        self.assertEqual(result, expected_uri)

    def test_getTargetUri(self):
        """Checks the getTargetUri method."""
        self._check_getTargetUri("file:///home/filename.ext.size.scaled_res.scaledproxy.mkv",
                                 "file:///home/filename.ext")
        self._check_getTargetUri("file:///home/filename.ext.size.proxy.mkv",
                                 "file:///home/filename.ext")
        self._check_getTargetUri("file:///home/file.name.mp4.1927006.1280x720.scaledproxy.mkv",
                                 "file:///home/file.name.mp4")
        self._check_getTargetUri("file:///home/file.name.mp4.1927006.proxy.mkv",
                                 "file:///home/file.name.mp4")

    def _check_getProxyUri(self, asset_uri, expected_uri, size=10, scaled=False, scaled_res=(1280, 720)):
        app = common.create_pitivi_mock()
        manager = app.proxy_manager

        asset = mock.Mock()
        asset.get_id.return_value = asset_uri
        with mock.patch.object(manager, "_scale_asset_resolution") as s_res:
            s_res.return_value = scaled_res
            with mock.patch("pitivi.utils.proxy.Gio.File") as gio:
                gio.new_for_uri.return_value = gio
                gio.query_info().get_size.return_value = size

                result = manager.getProxyUri(asset, scaled=scaled)
                self.assertEqual(result, expected_uri)

    def test_getProxyUri(self):
        """Checks the getProxyUri method."""
        self._check_getProxyUri("file:///home/file.name.mp4",
                                "file:///home/file.name.mp4.10.proxy.mkv")
        self._check_getProxyUri("file:///home/file.name.mp4",
                                "file:///home/file.name.mp4.10.1280x720.scaledproxy.mkv",
                                scaled=True)