import os
import requests
import unittest

from . import AsyncClient
from . import Client
from . import InteropError
from . import Mission
from . import Target
from . import Telemetry

# These tests run against a real interop server.
# The server be loaded with the data from the test fixture in
# server/fixtures/test_fixture.yaml.

# Set these environmental variables to the proper values
# if the defaults are not correct.
server = os.getenv('TEST_INTEROP_SERVER', 'http://localhost:8000')
username = os.getenv('TEST_INTEROP_USER', 'testuser')
password = os.getenv('TEST_INTEROP_USER_PASS', 'testpass')
admin_username = os.getenv('TEST_INTEROP_ADMIN', 'testadmin')
admin_password = os.getenv('TEST_INTEROP_ADMIN_PASS', 'testpass')


class TestClientLoggedOut(unittest.TestCase):
    """Test the portions of the Client class used before login."""

    def test_login(self):
        """Simple login test."""
        # Simply creating a Client causes a login.
        # If it doesn't raise an exception, it worked!
        Client(server, username, password)
        AsyncClient(server, username, password)

    def test_bad_login(self):
        """Bad login raises exception"""
        with self.assertRaises(InteropError):
            Client(server, "foo", "bar")
        with self.assertRaises(InteropError):
            AsyncClient(server, "foo", "bar")

    def test_timeout(self):
        """Test connection timeout"""
        # We are assuming that there is no machine at this address.
        addr = "http://10.255.255.254"
        timeout = 0.0001
        with self.assertRaises(requests.Timeout):
            Client(addr, username, password, timeout=timeout)
        with self.assertRaises(requests.Timeout):
            AsyncClient(addr, username, password, timeout=timeout)


class TestClient(unittest.TestCase):
    """Test the Client class.
    The Client class is a very thin wrapper, so there is very little to test.
    """

    def setUp(self):
        """Create a logged in Client."""
        # Create an admin client to clear cache.
        client = Client(server, admin_username, admin_password)
        client.get('/api/clear_cache')

        # Test rest with non-admin clients.
        self.client = Client(server, username, password)
        self.async_client = AsyncClient(server, username, password)

    def test_get_missions(self):
        """Test getting missions."""
        missions = self.client.get_missions()
        async_missions = self.async_client.get_missions().result()

        # Check one mission returned.
        self.assertEqual(1, len(missions))
        self.assertEqual(1, len(async_missions))
        # Check a few fields.
        self.assertTrue(missions[0].active)
        self.assertTrue(async_missions[0].active)
        self.assertEqual(1, missions[0].id)
        self.assertEqual(1, async_missions[0].id)
        self.assertEqual(38.14792, missions[0].home_pos.latitude)
        self.assertEqual(38.14792, async_missions[0].home_pos.latitude)

    def test_post_telemetry(self):
        """Test sending some telemetry."""
        t = Telemetry(latitude=38,
                      longitude=-76,
                      altitude_msl=100,
                      uas_heading=90)

        # Raises an exception on error.
        self.client.post_telemetry(t)
        self.async_client.post_telemetry(t).result()

    def test_post_bad_telemetry(self):
        """Test sending some (incorrect) telemetry."""
        t0 = Telemetry(latitude=38,
                       longitude=-76,
                       altitude_msl=100,
                       uas_heading=90)
        # The Telemetry constructor prevents us from passing invalid
        # values, but we can still screw things up in an update
        t0.latitude = 'baz'
        with self.assertRaises(InteropError):
            self.client.post_telemetry(t0)
        with self.assertRaises(InteropError):
            self.async_client.post_telemetry(t0).result()

        # We only accept Telemetry objects (or objects that behave like
        # Telemetry, not dicts.
        t1 = {
            'latitude': 38,
            'longitude': -76,
            'altitude_msl': 100,
            'uas_heading': 90
        }
        with self.assertRaises(AttributeError):
            self.client.post_telemetry(t1)
        with self.assertRaises(AttributeError):
            self.async_client.post_telemetry(t1).result()

    def test_get_obstacles(self):
        """Test getting obstacles."""
        stationary, moving = self.client.get_obstacles()
        async_future = self.async_client.get_obstacles()
        async_stationary, async_moving = async_future.result()

        # No exceptions is a good sign, let's see if the data matches the fixture.
        self.assertEqual(2, len(stationary))
        self.assertEqual(2, len(async_stationary))
        self.assertEqual(1, len(moving))
        self.assertEqual(1, len(async_moving))

        # Lat, lon, and altitude of the moving obstacles change, so we don't
        # check those.
        self.assertEqual(50, moving[0].sphere_radius)
        self.assertEqual(50, async_moving[0].sphere_radius)

        radii = [o.cylinder_radius for o in stationary]
        async_radii = [o.cylinder_radius for o in async_stationary]
        self.assertIn(50, radii)
        self.assertIn(50, async_radii)
        self.assertIn(150, radii)
        self.assertIn(150, async_radii)

        heights = [o.cylinder_height for o in stationary]
        self.assertIn(300, heights)
        self.assertIn(200, heights)
        async_heights = [o.cylinder_height for o in async_stationary]
        self.assertIn(300, async_heights)
        self.assertIn(200, async_heights)

    def test_targets(self):
        """Test target workflow."""
        # Post a target gets an updated target.
        target = Target(type='standard')
        post_target = self.client.post_target(target)
        async_post_target = self.client.post_target(target)

        self.assertIsNotNone(post_target.id)
        self.assertIsNotNone(async_post_target.id)
        self.assertIsNotNone(post_target.user)
        self.assertIsNotNone(async_post_target.user)
        self.assertEqual('standard', post_target.type)
        self.assertEqual('standard', async_post_target.type)
        self.assertNotEqual(post_target.id, async_post_target.id)

        # Get targets.
        get_target = self.client.get_target(post_target.id)
        async_get_target = self.async_client.get_target(
            async_post_target.id).result()
        get_targets = self.client.get_targets()
        async_get_targets = self.async_client.get_targets().result()

        self.assertEquals(post_target, get_target)
        self.assertEquals(async_post_target, async_get_target)
        self.assertIn(post_target, get_targets)
        self.assertIn(async_post_target, async_get_targets)

        # Update target.
        post_target.shape = 'circle'
        async_post_target.shape = 'circle'
        put_target = self.client.put_target(post_target.id, post_target)
        async_put_target = self.async_client.put_target(
            async_post_target.id, async_post_target).result()

        self.assertEquals(post_target, put_target)
        self.assertEquals(async_post_target, async_put_target)

        # Upload target image.
        test_image_filepath = os.path.join(
            os.path.dirname(__file__), "testdata/A.jpg")
        with open(test_image_filepath, 'rb') as f:
            image_data = f.read()
        self.client.put_target_image(post_target.id, image_data)
        self.async_client.put_target_image(async_post_target.id,
                                           image_data).result()

        # Get the target image.
        get_image = self.client.get_target_image(post_target.id)
        async_get_image = self.async_client.get_target_image(
            async_post_target.id).result()
        self.assertEquals(image_data, get_image)
        self.assertEquals(image_data, async_get_image)

        # Delete the target image.
        self.client.delete_target_image(post_target.id)
        self.async_client.delete_target_image(async_post_target.id).result()
        with self.assertRaises(InteropError):
            self.client.get_target_image(post_target.id)
        with self.assertRaises(InteropError):
            self.async_client.get_target_image(async_post_target.id).result()

        # Delete target.
        self.client.delete_target(post_target.id)
        self.async_client.delete_target(async_post_target.id).result()

        self.assertNotIn(post_target, self.client.get_targets())
        self.assertNotIn(async_post_target,
                         self.async_client.get_targets().result())
