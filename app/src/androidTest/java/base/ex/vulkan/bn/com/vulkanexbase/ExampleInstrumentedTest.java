package base.ex.vulkan.bn.com.vulkanexbase;
import android.content.Context;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;
import org.junit.Test;
import org.junit.runner.RunWith;
import static org.junit.Assert.*;
/**
 * Instrumentation test, which will execute on an Android devicePointer.
 *
 * @see <a href="http:
 */
@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {
    @Test
    public void useAppContext() throws Exception {
        Context appContext = InstrumentationRegistry.getTargetContext();
        assertEquals("base.ex.vulkan.bn.com.vulkanexbase", appContext.getPackageName());
    }
}
